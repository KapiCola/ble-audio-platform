# Test Logs

This directory contains logs collected during BLE Audio platform evaluation.

## Directory Structure

- `01_Lab_Tests/`
  Laboratory measurements with separate logs for source and sink:
  - `Source_Logs/app_core.log`
  - `Sink_Logs/app_core.log`
- `02_Real_World_Tests/`
  Real-world test scenario with the available sink-side application-core log:
  - `Sink_Logs/app_core.log`
- `03_Audio_Transmission_test/`
  Focused audio transmission check with direct source and sink logs:
  - `source.log`
  - `sink.log`

## Notes

Due to memory constraints, logging on the Network Core was disabled.
All provided logs come from the Application Core only.

Not every scenario contains both source and sink logs; the directory contents reflect the logs that were captured and preserved for each test run.
