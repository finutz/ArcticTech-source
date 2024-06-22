#pragma once

class CViewSetup;

class CWorld {
public:
	void Modulation();
	void Fog();
	void DisablePostProcessing( );
	void RemoveBlood();
	void SkyBox();
	void ProcessCamera(CViewSetup* view_setup);
	void Smoke();
	void Crosshair();
	void SunDirection();
};

extern CWorld* World;