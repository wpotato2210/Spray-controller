AGENTS.md

Spray Controller Automation Agents

Purpose

Automation agents assist with:
	•	Documentation updates
	•	Frozen interface validation
	•	Test script generation
	•	Monitoring system events

Tasks

Agent Name	Task Description	Trigger / Schedule
DocUpdater	Updates .md files with new interface or wiring info	On commit / manual trigger
InterfaceValidator	Checks frozen module interfaces against code	Nightly / pre-release
TestScriptGen	Generates synthetic event tests for SEE/THINK/DO	Manual or CI pipeline
EventMonitor	Logs system events for analysis	Continuous during operation

Placeholders
	•	Workflow and execution scripts
	•	Integration with CI/CD or local development tools
	•	Automated calibration routines