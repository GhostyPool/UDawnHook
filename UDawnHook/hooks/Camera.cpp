#include "pch.h"
#include "hooks.h"
#include "unreal/APlayerCameraManager.h"
#include "unreal/USkeletalMesh.h"
#include "unreal/ABatesCharacter.h"
#include "unreal/UObject.h"
#include "plugin/Menu.h"
#include "helper/eKeyboardMan.h"
#include "plugin/Settings.h"

using namespace Hooks;
using namespace Hooks::Camera;

static void HandleFreeCam(APlayerCameraManager* TheCamera)
{
	std::lock_guard<std::mutex> lock(TheMenu->mtx_freecamera);

	if (TheMenu->freeCamSharedData.bFreeCamEnabled)
	{
		if (TheMenu->freeCamSharedData.bFreeCamShouldReset)
		{
			TheMenu->freeCamSharedData.FreeCameraPos = TheCamera->CameraCache.POV.Location;
			TheMenu->freeCamSharedData.FreeCameraRot = TheCamera->CameraCache.POV.Rotation;
			TheMenu->freeCamSharedData.fFreeCameraFOV = TheCamera->CameraCache.POV.FOV;
			TheMenu->freeCamSharedData.bFreeCamModified = false;
			TheMenu->freeCamSharedData.bFreeCamShouldReset = false;
		}

		FMatrix FreeCameraMatrix = FMatrix(TheMenu->freeCamSharedData.FreeCameraRot);

		FVector forward = FreeCameraMatrix.GetForward();
		FVector right = FreeCameraMatrix.GetRight();

		float speed = TheMenu->freeCamSharedData.fFreeCamSpeed;
		float rotationSpeed = TheMenu->freeCamSharedData.fFreeCamRotationSpeed;

		if (!TheMenu->m_bWantsKeyboard.load(std::memory_order_relaxed))
		{
			std::lock_guard<std::mutex> keys_lock(SettingsMgr->mtx_keys);

			bool bModified = false;

			//forward
			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyForward) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos += forward * speed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyBackward) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos -= forward * speed * Hooks::deltaTime;
				bModified |= true;
			}

			//right
			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyRight) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos += right * speed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyLeft) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos -= right * speed * Hooks::deltaTime;
				bModified |= true;
			}

			//up
			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyUp) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos.Z += speed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyDown) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraPos.Z -= speed * Hooks::deltaTime;
				bModified |= true;
			}

			//rotation
			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyPitchPlus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Pitch += rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyPitchMinus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Pitch -= rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyYawPlus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Yaw += rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyYawMinus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Yaw -= rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyRollPlus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Roll += rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}

			if (eKeyboardMan::GetKeyState(SettingsMgr->keys.freeCam.iFreeCameraKeyRollMinus) & KEY_HELD)
			{
				TheMenu->freeCamSharedData.FreeCameraRot.Roll -= rotationSpeed * Hooks::deltaTime;
				bModified |= true;
			}


			if (bModified)
				TheMenu->freeCamSharedData.bFreeCamModified = true;
		}

		//update actual game camera
		TheCamera->CameraCache.POV.Location = TheMenu->freeCamSharedData.FreeCameraPos;
		TheCamera->CameraCache.POV.Rotation = TheMenu->freeCamSharedData.FreeCameraRot;
		TheCamera->CameraCache.POV.FOV = TheMenu->freeCamSharedData.fFreeCameraFOV;
	}
	else
	{
		if (!TheMenu->freeCamSharedData.bFreeCamModified)
		{
			TheMenu->freeCamSharedData.FreeCameraPos = TheCamera->CameraCache.POV.Location;
			TheMenu->freeCamSharedData.FreeCameraRot = TheCamera->CameraCache.POV.Rotation;
			TheMenu->freeCamSharedData.fFreeCameraFOV = TheCamera->CameraCache.POV.FOV;
		}
	}
}

static void HandleFirstPersonCam(APlayerCameraManager* TheCamera)
{
	ABatesCharacter& character = *reinterpret_cast<ABatesCharacter*>(pCharacter);
	Menu::FPCamSharedData fpCamData;
	{
		std::lock_guard<std::mutex> lock(TheMenu->mtx_firstpersoncamera);
		fpCamData = TheMenu->fpCamSharedData;
	}

	if (fpCamData.bFPCamEnabled)
	{
		TheCamera->CameraCache.POV.Location = character.body->GetBoneLocation(L"cameraJoint");

		TheCamera->CameraCache.POV.FOV = fpCamData.fFPCamFOV;

		TheCamera->CameraCache.POV.PerspectiveNearClipPlane = fpCamData.fFPCamNearClipPlane;
	}
}

void Hooks::Camera::DoUpdateCamera_Hook(int64 a1, float deltaTime)
{
	if (oDoUpdateCamera)
		oDoUpdateCamera(a1, deltaTime);

	TheCamera = reinterpret_cast<APlayerCameraManager*>(a1);

	UObject*& ViewTarget = TheCamera->ViewTarget.Target;

	//Free cam
	HandleFreeCam(TheCamera);

	static FName explorationCamera("BP_ExplorationCamera_C", FNAME_Add);
	static FName initialFramingCamera("BP_InitialFramingCamera_Mover_C", FNAME_Add);

	//First person cam
	if (pCharacter && (ViewTarget && (ViewTarget->Class->GetFName() == explorationCamera || ViewTarget->Class->GetFName() == initialFramingCamera)))
		HandleFirstPersonCam(TheCamera);
}