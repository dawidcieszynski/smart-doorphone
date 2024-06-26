#include "pulse_meter_sensor.h"
#include <utility>
#include "esphome/core/log.h"

namespace esphome
{
  namespace pulse_meter
  {

    static const char *const TAG = "pulse_meter";

    void PulseMeterSensor::set_total_pulses(uint32_t pulses)
    {
      this->total_pulses_ = pulses;
      if (this->total_sensor_ != nullptr)
      {
        this->total_sensor_->publish_state(this->total_pulses_);
      }
    }

    void PulseMeterSensor::setup()
    {
      this->pin_->setup();
      this->isr_pin_ = pin_->to_isr();

      // Set the last processed edge to now for the first timeout
      this->last_processed_edge_us_ = micros();

      if (this->filter_mode_ == FILTER_EDGE)
      {
        this->pin_->attach_interrupt(PulseMeterSensor::edge_intr, this, gpio::INTERRUPT_FALLING_EDGE);
      }
      else if (this->filter_mode_ == FILTER_PULSE)
      {
        this->pin_->attach_interrupt(PulseMeterSensor::pulse_intr, this, gpio::INTERRUPT_ANY_EDGE);
      }
    }

    void PulseMeterSensor::loop()
    {
      // Reset the count in get before we pass it back to the ISR as set
      this->get_->count_ = 0;

      // Swap out set and get to get the latest state from the ISR
      // The ISR could interrupt on any of these lines and the results would be consistent
      auto *temp = this->set_;
      this->set_ = this->get_;
      this->get_ = temp;

      // Check if we detected a pulse this loop
      if (this->get_->count_ > 0)
      {
        // Keep a running total of pulses if a total sensor is configured
        if (this->total_sensor_ != nullptr)
        {
          this->total_pulses_ += this->get_->count_;
          const uint32_t total = this->total_pulses_;
          this->total_sensor_->publish_state(total);
        }

        // We need to detect at least two edges to have a valid pulse width
        switch (this->meter_state_)
        {
        case MeterState::INITIAL:
        case MeterState::TIMED_OUT:
        {
          this->meter_state_ = MeterState::WAITING;
          this->log_state();
        }
        break;
        case MeterState::WAITING:
        {
          uint32_t delta_us = this->get_->last_detected_edge_us_ - this->last_processed_edge_us_;
          if (delta_us > initial_pulse_us_)
          {
            ESP_LOGD(TAG, "Initial pulse %" PRIu32 "ms detected", delta_us / 1000);
            // start counting
            this->total_pulses_ = 0;
            this->get_->count_ = 0;
            this->set_->count_ = 0;
            this->meter_state_ = MeterState::RUNNING;
            this->log_state();
          }
        }
        break;
        case MeterState::RUNNING:
        {
          uint32_t delta_us = this->get_->last_detected_edge_us_ - this->last_processed_edge_us_;
          ESP_LOGD(TAG, "Countable pulse(s) %" PRIu32 "ms detected (count: %" PRIu32 ")", delta_us / 1000, this->total_pulses_);
          float pulse_width_us = delta_us / float(this->get_->count_);
          this->publish_state((60.0f * 1000000.0f) / pulse_width_us);
        }
        break;
        }

        this->last_processed_edge_us_ = this->get_->last_detected_edge_us_;
      }
      // No detected edges this loop
      else
      {
        const uint32_t now = micros();
        const uint32_t time_since_valid_edge_us = now - this->last_processed_edge_us_;

        switch (this->meter_state_)
        {
        // Running and initial states can timeout
        case MeterState::INITIAL:
        case MeterState::WAITING:
        case MeterState::RUNNING:
        {
          if (time_since_valid_edge_us > this->timeout_us_)
          {
            this->meter_state_ = MeterState::TIMED_OUT;
            this->log_state();

            ESP_LOGD(TAG, "Timeout for %" PRIu32 "ms (count: %" PRIu32 ")", time_since_valid_edge_us / 1000, this->total_pulses_);
            ESP_LOGD(TAG, "Pulses %" PRIu32 "", this->last_pulse_index);

            for (size_t i = 0; i < this->last_pulse_index; i++)
            {
              ESP_LOGD(TAG, " Pulse %" PRIu32 "us (%" PRIu32 "us pause)", this->pulses[i].length, this->pulses[i].pause);
            }

            // analyze pulses (low active time/high pause time)
            // 202.295/2.291ms initial pulse
            // 22/224 us flat number pulses
            // 190us/234us ring
            if (this->last_pulse_index > 0)
            {
              auto is_initial_pulse = [this](const Pulse &pulse)
              {
                return (this->initial_pulse_us_ * 0.9) < pulse.length && pulse.length < (this->initial_pulse_us_ * 1.1);
              };

              bool initial_pulse = false;
              auto flat_no_counter = 0;
              ESP_LOGD(TAG, "Processing pulses");
              for (size_t i = 0; i < this->last_pulse_index; i++)
              {
                ESP_LOGD(TAG, "  %" PRIu32 "", i);
                if (!initial_pulse && is_initial_pulse(this->pulses[i]))
                {
                  initial_pulse = true;
                  ESP_LOGD(TAG, "Initial pulse %" PRIu32 "ms detected", this->pulses[i].length / 1000);
                  continue;
                }

                if (initial_pulse)
                {
                  ESP_LOGD(TAG, "Increase Flat number");
                  flat_no_counter++;
                }

                if (this->pulses[i].length > 0 && this->pulses[i].pause > 1000) // flat number timeout
                {
                  ESP_LOGD(TAG, "Flat number timeout");
                  break;
                }
              }

              if (flat_no_counter > 0)
              {
                ESP_LOGD(TAG, "Flat %" PRIu32 " called", flat_no_counter);

                this->publish_state(flat_no_counter);
                this->clear_pulses();
                break;
              }
            }

            this->publish_state(0.0f);
            this->clear_pulses();
          }
        }
        break;
        default:
          break;
        }
      }
    }

    float PulseMeterSensor::get_setup_priority() const { return setup_priority::DATA; }

    void PulseMeterSensor::dump_config()
    {
      LOG_SENSOR("", "Pulse Meter", this);
      LOG_PIN("  Pin: ", this->pin_);
      if (this->filter_mode_ == FILTER_EDGE)
      {
        ESP_LOGCONFIG(TAG, "  Filtering falling edges less than %" PRIu32 " µs apart", this->filter_us_);
      }
      else
      {
        ESP_LOGCONFIG(TAG, "  Filtering pulses shorter than %" PRIu32 " µs", this->filter_us_);
      }
      ESP_LOGCONFIG(TAG, "  Assuming 0 pulses/min after not receiving a pulse for %" PRIu32 "s",
                    this->timeout_us_ / 1000000);
      ESP_LOGCONFIG(TAG, "  Initial pulse length: %" PRIu32 "ms",
                    this->initial_pulse_us_ / 1000);
    }

    void PulseMeterSensor::log_state()
    {
      switch (this->meter_state_)
      {
      case MeterState::INITIAL:
        ESP_LOGD(TAG, "State INITIAL");
        break;
      case MeterState::WAITING:
        ESP_LOGD(TAG, "State WAITING");
        break;
      case MeterState::RUNNING:
        ESP_LOGD(TAG, "State RUNNING");
        break;
      case MeterState::TIMED_OUT:
        ESP_LOGD(TAG, "State TIMED_OUT");
        break;
      default:
        ESP_LOGD(TAG, "State ?");
        break;
      }
    }

    void PulseMeterSensor::clear_pulses()
    {
      for (size_t i = 0; i < this->last_pulse_index; i++)
      {
        this->pulses[i].length = 0;
        this->pulses[i].pause = 0;
      }
      this->last_pulse_index = 0;
    }

    void PulseMeterSensor::dump_logs()
    {
      if (this->logs.empty())
      {
        return;
      }

      ESP_LOGD(TAG, "  Logs: %" PRIu32 "", this->logs.length());
      ESP_LOGD(TAG, "  %s", this->logs.c_str());
      this->logs.clear();
    }

    void IRAM_ATTR PulseMeterSensor::edge_intr(PulseMeterSensor *sensor)
    {
      // This is an interrupt handler - we can't call any virtual method from this method
      // Get the current time before we do anything else so the measurements are consistent
      const uint32_t now = micros();

      if ((now - sensor->last_edge_candidate_us_) >= sensor->filter_us_)
      {
        sensor->pulses[sensor->last_pulse_index].length = now - sensor->last_edge_candidate_us_;
        sensor->last_pulse_index++;

        sensor->last_edge_candidate_us_ = now;
        sensor->set_->last_detected_edge_us_ = now;
        sensor->set_->count_++;
      }
    }

    void IRAM_ATTR PulseMeterSensor::pulse_intr(PulseMeterSensor *sensor)
    {
      // This is an interrupt handler - we can't call any virtual method from this method
      // Get the current time before we do anything else so the measurements are consistent
      const uint32_t now = micros();
      bool pin_val = sensor->isr_pin_.digital_read();

      // sensor->logs += std::to_string(pin_val) + " ";

      // A pulse occurred faster than we can detect
      if (sensor->last_pin_val_ == pin_val)
      {
        // If we haven't reached the filter length yet we need to reset our last_intr_ to now
        // otherwise we can consider this noise as the "pulse" was certainly less than filter_us_
        if (now - sensor->last_intr_ < sensor->filter_us_)
        {
          sensor->last_intr_ = now;
        }
      }
      else
      {
        // Check if the last interrupt was long enough in the past
        if (now - sensor->last_intr_ > sensor->filter_us_)
        {
          // Low pulse of filter length now falling (therefore last_intr_ was the rising edge)
          if (!sensor->in_pulse_ && !pin_val)
          {
            // shouldn't happen
            if (sensor->last_pulse_index == 0)
            {
              // sensor->logs += " ! ";
              sensor->last_pulse_index++;
            }

            sensor->pulses[sensor->last_pulse_index - 1].pause = now - sensor->get_->last_detected_edge_us_;

            sensor->last_edge_candidate_us_ = sensor->last_intr_;
            sensor->in_pulse_ = true;
          }
          // High pulse of filter length now rising (therefore last_intr_ was the falling edge)
          else if (sensor->in_pulse_ && pin_val)
          {
            sensor->pulses[sensor->last_pulse_index].length = now - sensor->last_edge_candidate_us_;
            sensor->last_pulse_index++;

            sensor->set_->last_detected_edge_us_ = sensor->last_edge_candidate_us_;
            sensor->set_->count_++;
            sensor->in_pulse_ = false;
          }
          else
          {
            // sensor->logs += " ? ";
          }
        }

        sensor->last_intr_ = now;
        sensor->last_pin_val_ = pin_val;
      }
    }

  } // namespace pulse_meter
} // namespace esphome