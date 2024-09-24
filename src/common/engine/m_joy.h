#ifndef M_JOY_H
#define M_JOY_H

#include "basics.h"
#include "tarray.h"
#include "c_cvars.h"
#include "printf.h"
#include "i_time.h"

#include <algorithm>

enum EJoyAxis
{
	JOYAXIS_None = -1,
	JOYAXIS_Yaw,
	JOYAXIS_Pitch,
	JOYAXIS_Forward,
	JOYAXIS_Side,
	JOYAXIS_Up,
//	JOYAXIS_Roll,		// Ha ha. No roll for you.
	NUM_JOYAXIS,
};

// Generic configuration interface for a controller.
struct IJoystickConfig
{
protected:
	double rumblePast = 0;
	const double rumbleDecay = 0.4;
	double leftRumble = 0, rightRumble = 0;

public:
	virtual ~IJoystickConfig() = 0;

	virtual FString GetName() = 0;
	virtual float GetSensitivity() = 0;
	virtual void SetSensitivity(float scale) = 0;

	virtual int GetNumAxes() = 0;
	virtual float GetAxisDeadZone(int axis) = 0;
	virtual EJoyAxis GetAxisMap(int axis) = 0;
	virtual const char *GetAxisName(int axis) = 0;
	virtual float GetAxisScale(int axis) = 0;

	virtual void SetAxisDeadZone(int axis, float zone) = 0;
	virtual void SetAxisMap(int axis, EJoyAxis gameaxis) = 0;
	virtual void SetAxisScale(int axis, float scale) = 0;

	virtual bool GetEnabled() = 0;
	virtual void SetEnabled(bool enabled) = 0;

	virtual bool AllowsEnabledInBackground() = 0;
	virtual bool GetEnabledInBackground() = 0;
	virtual void SetEnabledInBackground(bool enabled) = 0;

	// Used by the saver to not save properties that are at their defaults.
	virtual bool IsSensitivityDefault() = 0;
	virtual bool IsAxisDeadZoneDefault(int axis) = 0;
	virtual bool IsAxisMapDefault(int axis) = 0;
	virtual bool IsAxisScaleDefault(int axis) = 0;

	virtual void SetDefaultConfig() = 0;
	virtual FString GetIdentifier() = 0;

	virtual void SetRumbleInternal(float left, float right)
	{
		Printf(TEXTCOLOR_RED "Rumble not implemented");
	}

	void SetRumble(float left, float right)
	{
		SetRumbleInternal(left, right);
		rumblePast = I_msTimeF() / 1000.;
	}

	void AddRumble(float left, float right)
	{
		SetRumbleInternal(leftRumble + left, rightRumble + right);
		rumblePast = I_msTimeF() / 1000.;
	}

	void UpdateRumble()
	{
		double tm = I_msTimeF() / 1000.0;
		double te = std::clamp((float)(tm - (rumblePast + 0.04)), 0.0f, 10.0f);
		leftRumble = std::clamp(leftRumble - (te * (1.0 / rumbleDecay)), 0.0, 1.0);
		rightRumble = std::clamp(rightRumble - (te * (1.0 / rumbleDecay)), 0.0, 1.0);

	}
};

EXTERN_CVAR(Bool, use_joystick);
EXTERN_CVAR(Bool, joy_feedback);
EXTERN_CVAR(Float, joy_feedback_scale);

bool M_LoadJoystickConfig(IJoystickConfig *joy);
void M_SaveJoystickConfig(IJoystickConfig *joy);

void Joy_GenerateButtonEvents(int oldbuttons, int newbuttons, int numbuttons, int base);
void Joy_GenerateButtonEvents(int oldbuttons, int newbuttons, int numbuttons, const int *keys);
int Joy_XYAxesToButtons(double x, double y);
double Joy_RemoveDeadZone(double axisval, double deadzone, uint8_t *buttons);

void I_SetJoystickRumble(double factor);

// These ought to be provided by a system-specific i_input.cpp.
void I_GetAxes(float axes[NUM_JOYAXIS]);
void I_GetJoysticks(TArray<IJoystickConfig *> &sticks);
IJoystickConfig *I_UpdateDeviceList();
extern void UpdateJoystickMenu(IJoystickConfig *);

#endif
