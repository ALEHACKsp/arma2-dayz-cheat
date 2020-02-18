#include "arma2.h"
#include <stdio.h>
#include <Subauth.h>
#include <time.h>
#include <iostream>
#include <string>
#include "Xor.h"
#include "offsets.h"
#include "string.h"
#include "Minhook/include/MinHook.h"
#include <chrono>
#include "Script.h"
#include "Console.h"

Logging* Log;

std::chrono::high_resolution_clock timer;
auto delay = timer.now() + std::chrono::milliseconds(2000);

HRESULT __stdcall EndScene(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(__stdcall* EndScene_t)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* oEndScene)             (LPDIRECT3DDEVICE9 pDev);
typedef int (WINAPI *Present_t)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
Present_t oPresent;
EndScene_t pEndScene;
bool thermal = false;
bool nightvision = false;
bool bunlock = false;
#define IS_BAD(X) IsBadReadPtr( (void*)X, sizeof( X ) )

bool drawMenu = false;

LPD3DXFONT m_pFont;
ID3DXLine* m_rectLine;
DWORD dwModBase;
char hurennigger[5012];
D3DXVECTOR3 LocalPos; // pos
DWORD* dwpVTableStart = NULL; // d3d9.dll device vtable
bool Init = 0;

// W 2 S stuff
D3DXVECTOR3 InvViewRight;
D3DXVECTOR3 InvViewUp;
D3DXVECTOR3 InvViewForward;
D3DXVECTOR3 InvViewTranslation;
D3DXVECTOR3 ViewPortMatrix;
D3DXVECTOR3 ProjD1;
D3DXVECTOR3 ProjD2;
// end

// colors
const D3DCOLOR txtPink = D3DCOLOR_ARGB(255, 255, 0, 255);   // pink
const D3DCOLOR txtGreen = D3DCOLOR_ARGB(255, 20, 222, 20);  //green
const D3DCOLOR txtBlue = D3DCOLOR_ARGB(255, 0, 255, 255);  //cyan
const D3DCOLOR txtRed = D3DCOLOR_ARGB(255, 255, 0, 0);  //red
const D3DCOLOR txtGrey = D3DCOLOR_ARGB(255, 222, 222, 222);  //granny
const D3DCOLOR txtDarkGrey = D3DCOLOR_ARGB(255, 142, 142, 182);  //darkgranny
const D3DCOLOR txtYellow = D3DCOLOR_ARGB(255, 255, 255, 0);
// colors end

typedef struct _PEB_LDR_DATA
{
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LDR_MODULE {

	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;

} LDR_MODULE, *PLDR_MODULE;


DWORD D3D9VTable()
{
	DWORD dwObjBase = (DWORD)LoadLibraryA(STR12);
	while (dwObjBase++ < dwObjBase + 0x127850)
	{
		if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7 && (*(WORD*)(dwObjBase + 0x06)) == 0x8689 && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689)
		{
			dwObjBase += 2;
			break;
		}
	}
	return (dwObjBase);
}

void DrawRect(LPDIRECT3DDEVICE9 Device_t, int X, int Y, int L, int H, D3DCOLOR color)
{
	D3DRECT rect = { X, Y, X + L, Y + H };
	Device_t->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0); // bei Google gibt’s n?heres
}

void DrawLine(ID3DXLine* pLine, float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DXVECTOR2 Vector2[2];
	Vector2[0].x = x1;
	Vector2[0].y = y1;
	Vector2[1].x = x2;
	Vector2[1].y = y2;
	pLine->Draw(Vector2, 2, color);
}

void DrawFont(int X, int Y, D3DCOLOR Color,  char *format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	RECT FontRect = { X, Y, X + 120, Y + 16 };
	m_pFont->DrawText(NULL, buffer, -1, &FontRect, DT_NOCLIP, Color); // Zeichnen
	va_end(args);
}

bool unlockvei = false;

void UpdateView()
{
	if (GetKeyState(0x75) & 0x8000)
	{
		bunlock = true;
	}

	if (GetKeyState(0x76) & 0x8000 )
	{
		thermal = !thermal;
	}
	if (GetKeyState(0x77) & 0x8000 )
	{
		nightvision = !nightvision;
	}

	const D3DCOLOR txtGreen = D3DCOLOR_ARGB(255, 255, 0, 255); // Alpha, Rot, Gr?n, Blau
	DWORD *dwTransformations = (DWORD*)P_Transformation;
	DWORD *dwTransData = (DWORD*)(*dwTransformations + 0x90);
	InvViewRight = *(D3DXVECTOR3*)(*dwTransData + 0x4);
	InvViewUp = *(D3DXVECTOR3*)(*dwTransData + 0x10);
	InvViewForward = *(D3DXVECTOR3*)(*dwTransData + 0x1C);
	InvViewTranslation = *(D3DXVECTOR3*)(*dwTransData + 0x28);
	ViewPortMatrix = *(D3DXVECTOR3*)(*dwTransData + 0x54);
	ProjD1 = *(D3DXVECTOR3*)(*dwTransData + 0xCC);
	ProjD2 = *(D3DXVECTOR3*)(*dwTransData + 0xD8);
}

D3DXVECTOR3 WorldToScreen(D3DXVECTOR3 in)
{
	D3DXVECTOR3 out, temp;

	D3DXVec3Subtract(&temp, &in, &InvViewTranslation);
	float x = D3DXVec3Dot(&temp, &InvViewRight);
	float y = D3DXVec3Dot(&temp, &InvViewUp);
	float z = D3DXVec3Dot(&temp, &InvViewForward);

	out.x = ViewPortMatrix.x * (1 + (x / ProjD1.x / z));
	out.y = ViewPortMatrix.y * (1 - (y / ProjD2.y / z));
	out.z = z;

	return out;
}

void UnlinkModule(char *szModule)
{
	DWORD dwPEB = 0, dwOffset = 0;
	PLIST_ENTRY pUserModuleHead, pUserModule;
	PPEB_LDR_DATA pLdrData;
	PLDR_MODULE pLdrModule;
	PUNICODE_STRING lpModule;
	char szModuleName[512];
	int i = 0, n = 0;

	_asm
	{
		pushad
		mov eax, fs: [48]
		mov dwPEB, eax
		popad
	}

	pLdrData = (PPEB_LDR_DATA)(PDWORD)(*(PDWORD)(dwPEB + 12));

	for (; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InLoadOrderModuleList));
			dwOffset = 0;
			break;

		case 1:
			pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InMemoryOrderModuleList));
			dwOffset = 8;
			break;
		case 2:
			pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InInitializationOrderModuleList));
			dwOffset = 16;
			break;
		}

		while (pUserModule->Flink != pUserModuleHead)
		{
			pUserModule = pUserModule->Flink;
			lpModule = (PUNICODE_STRING)(((DWORD)(pUserModule)) + (36 - dwOffset));

			for (n = 0; n < (lpModule->Length) / 2 && n < 512; n++)
				szModuleName[n] = (CHAR)(*((lpModule->Buffer) + (n)));

			szModuleName[n] = '\0';
			if (strstr(szModuleName, szModule))
			{
				pUserModule->Blink->Flink = pUserModule->Flink;
				pUserModule->Flink->Blink = pUserModule->Blink;
			}
		}
	}
}

void HideModule(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;

	_asm
	{
		pushad;
		pushfd;
		mov eax, fs:[30h]                                       // PEB
			mov eax, [eax + 0Ch]                                      // PEB->ProcessModuleInfo
			mov dwPEB_LDR_DATA, eax                         // Save ProcessModuleInfo

			InLoadOrderModuleList :
		mov esi, [eax + 0Ch]                                      // ProcessModuleInfo->InLoadOrderModuleList[FORWARD]
			mov edx, [eax + 10h]                                      //  ProcessModuleInfo->InLoadOrderModuleList[BACKWARD]

			LoopInLoadOrderModuleList :
			lodsd                                                           //  Load First Module
			mov esi, eax                                    //  ESI points to Next Module
			mov ecx, [eax + 18h]                              //  LDR_MODULE->BaseAddress
			cmp ecx, hModule                                //  Is it Our Module ?
			jne SkipA                                       //  If Not, Next Please (@f jumps to nearest Unamed Lable @@:)
			mov ebx, [eax]                                          //  [FORWARD] Module
			mov ecx, [eax + 4]                        //  [BACKWARD] Module
			mov[ecx], ebx                                          //  Previous Module's [FORWARD] Notation, Points to us, Replace it with, Module++
			mov[ebx + 4], ecx                                        //  Next Modules, [BACKWARD] Notation, Points to us, Replace it with, Module--
			jmp InMemoryOrderModuleList                     //  Hidden, so Move onto Next Set
			SkipA :
		cmp edx, esi                                            //  Reached End of Modules ?
			jne LoopInLoadOrderModuleList           //  If Not, Re Loop

			InMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA                         //  PEB->ProcessModuleInfo
			mov esi, [eax + 14h]                                      //  ProcessModuleInfo->InMemoryOrderModuleList[START]
			mov edx, [eax + 18h]                                      //  ProcessModuleInfo->InMemoryOrderModuleList[FINISH]

			LoopInMemoryOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 10h]
			cmp ecx, hModule
			jne SkipB
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InInitializationOrderModuleList
			SkipB :
		cmp edx, esi
			jne LoopInMemoryOrderModuleList

			InInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA                              //  PEB->ProcessModuleInfo
			mov esi, [eax + 1Ch]                                               //  ProcessModuleInfo->InInitializationOrderModuleList[START]
			mov edx, [eax + 20h]                                               //  ProcessModuleInfo->InInitializationOrderModuleList[FINISH]

			LoopInInitializationOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 08h]
			cmp ecx, hModule
			jne SkipC
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp Finished
			SkipC :
		cmp edx, esi
			jne LoopInInitializationOrderModuleList

			Finished :
		popfd;
		popad;
	}
}

inline float Vectorl3(D3DVECTOR v) {
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline float Vectorl2(D3DXVECTOR2 v) {
	return (sqrt(v.x * v.x + v.y * v.y));
}

char* getCombi(EntryArray *entry)
{
	while (true)
	{
		if ((DWORD)entry > 1)
		{
			if (!IsBadReadPtr(entry->firstEntry, 4))
			{
				varEntry* en = entry->firstEntry;
				if (en->vTable == 12644640)
				{
					if (strcmp(en->name->string, STR11) == 0)
						return(en->value->value->string);
				}
				else
				{
					return 0;
				}
			}
		}
		entry += 1;
	}
}

int spacePerItem = 17;
BYTE a = 255;


void PlayerESP()
{

	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	NetworkManager pNetworkmanager = *(NetworkManager*)P_NetworkManager;
	if (!pWorld.world->cameraOn->unit) return;

	LocalPos = pWorld.world->cameraOn->unit->entityVisualState->coordinates;

	EntityTablePointer* ObjectPtr = (pWorld.world->entityTablePointer);

	if (!IsBadReadPtr(ObjectPtr, 0x8))
	{
		DWORD* entTable = *(DWORD**)ObjectPtr;
		if (!IsBadReadPtr(entTable, 4))
		{
			for (int i = 0; i < pWorld.world->entityTablePointer->objectTableSize; ++i)
			{
				EntityTable *eTable = *(EntityTable**)((entTable + (13 * i)));
				if (eTable)
				{
					Entity* pEntity = eTable->e;
					if (pEntity)
					{
						int dist = (Vectorl3(LocalPos - pEntity->entityVisualState->coordinates));

						if (dist < 2000)
						{
							if (strcmp(pEntity->cfgVehicle->objectType->string, STR10) == 0)
							{
								bool human = false;
								Unit* u = (Unit*)pEntity;
								if (u->playerID > 1)
									human = true;

								if (!human) continue;

								EntityVisualState* vis = pEntity->entityVisualState;
								if (vis)
								{
									char* name = STR15;
									D3DXVECTOR3 vPos = WorldToScreen(vis->coordinates);
									D3DXVECTOR3 vChestPos = WorldToScreen(vis->torsoCoordinates);
									D3DXVECTOR3 vHeadPos = WorldToScreen(vis->headCoordinates);
									if (vPos.z > 0.01)
									{

										if (!u->isDead)
										{
											char t[255];
											sprintf(t, STR8, dist);

											DrawFont(vHeadPos.x, vHeadPos.y - 20, txtRed, t);
											DrawFont(vHeadPos.x, vHeadPos.y, txtYellow, STR16);
											DrawFont(vChestPos.x, vChestPos.y, txtYellow, STR17);
										}
										else
										{
											char t[255];
											sprintf(t, STR8, dist);

											DrawFont(vHeadPos.x, vHeadPos.y, txtDarkGrey, t);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void AIESP()
{

	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	NetworkManager pNetworkmanager = *(NetworkManager*)P_NetworkManager;
	if (!pWorld.world->cameraOn->unit) return;

	LocalPos = pWorld.world->cameraOn->unit->entityVisualState->coordinates;

	EntityTablePointer* ObjectPtr = (pWorld.world->entityTablePointer);

	if (!IsBadReadPtr(ObjectPtr, 0x8))
	{
		DWORD* entTable = *(DWORD**)ObjectPtr;
		if (!IsBadReadPtr(entTable, 4))
		{
			for (int i = 0; i < pWorld.world->entityTablePointer->objectTableSize; ++i)
			{
				EntityTable *eTable = *(EntityTable**)((entTable + (13 * i)));
				if (eTable)
				{
					Entity* pEntity = eTable->e;
					if (pEntity)
					{
						int dist = (Vectorl3(LocalPos - pEntity->entityVisualState->coordinates));

						if (dist < 2000)
						{
							if (strcmp(pEntity->cfgVehicle->objectType->string, STR10) == 0)
							{
								bool human = false;
								Unit* u = (Unit*)pEntity;
								if (u->playerID == 1)
									human = true;

								if (!human) continue;

								EntityVisualState* vis = pEntity->entityVisualState;
								if (vis)
								{
									char* name = STR15;
									D3DXVECTOR3 vPos = WorldToScreen(vis->coordinates);
									D3DXVECTOR3 vChestPos = WorldToScreen(vis->torsoCoordinates);
									D3DXVECTOR3 vHeadPos = WorldToScreen(vis->headCoordinates);
									if (vPos.z > 0.01)
									{

										if (!u->isDead)
										{
											char t[255];
											sprintf(t, STR8, dist);

											DrawFont(vHeadPos.x, vHeadPos.y - 20, txtBlue, t);
										}
										else
										{
											char t[255];
											sprintf(t, STR8, dist);

											DrawFont(vHeadPos.x, vHeadPos.y, txtDarkGrey, t);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void VehicleESP()
{

	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	NetworkManager pNetworkmanager = *(NetworkManager*)P_NetworkManager;
	if (!pWorld.world->cameraOn->unit) return;

	LocalPos = pWorld.world->cameraOn->unit->entityVisualState->coordinates;

	EntityTablePointer* ObjectPtr = (pWorld.world->entityTablePointer);

	if (!IsBadReadPtr(ObjectPtr, 0x8))
	{
		DWORD* entTable = *(DWORD**)ObjectPtr;
		if (!IsBadReadPtr(entTable, 4))
		{
			for (int i = 0; i < pWorld.world->entityTablePointer->objectTableSize; ++i)
			{
				EntityTable *eTable = *(EntityTable**)((entTable + (13 * i)));
				if (eTable)
				{
					Entity* pEntity = eTable->e;
					if (pEntity)
					{
						int dist = (Vectorl3(LocalPos - pEntity->entityVisualState->coordinates));

						if (dist < 1500)
						{
							if (strcmp(pEntity->cfgVehicle->objectType->string, STR2) == 0 ||
								strcmp(pEntity->cfgVehicle->objectType->string, STR3) == 0 ||
								strcmp(pEntity->cfgVehicle->objectType->string, STR4) == 0 ||
								strcmp(pEntity->cfgVehicle->objectType->string, STR5) == 0 ||
								strcmp(pEntity->cfgVehicle->objectType->string, STR6) == 0 ||
								strcmp(pEntity->cfgVehicle->objectType->string, STR7) == 0)
							{
								Vehicle* pVehicle = (Vehicle*)pEntity;
								
								if (dist <= 5 && bunlock)
								{
									Unit* pUnit = (Unit*)pVehicle;
									pUnit->vehicleLocked = 1;
									bunlock = false;
								}

								D3DXVECTOR3 vPos = WorldToScreen(pEntity->entityVisualState->coordinates);
								if (vPos.z > 0.01)
								{
									char t[255];
									sprintf(t, STR1, pEntity->cfgVehicle->entityName->string, dist);
									DrawFont(vPos.x, vPos.y, txtGreen, t);
								}
							}
						}
					}
				}
			}
		}
	}
}

void NoFatigue()
{

	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		if (pWorld.world->cameraOn->unit->fatigue != 0.0f)
		{
			pWorld.world->cameraOn->unit->fatigue = 0.0f;
			pWorld.world->cameraOn->unit->breath = 0.0f;
			pWorld.world->cameraOn->unit->reticleWanderX = 0.0f;
			pWorld.world->cameraOn->unit->reticleWanderY = 0.0f;
		}

	}
}

void NoRecoil()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		if (pWorld.world->cameraOn->unit->customRecoilCoef != 0.0f)
			pWorld.world->cameraOn->unit->customRecoilCoef = 0.0f;


	}

}

void NoGrass()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		if (pWorld.world->grass != 50.0f)
			pWorld.world->grass = 50.0f;

	}
}

void RepairVehicle()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
			pWorld.world->cameraOn->unit->Repair();

			Vehicle* CurrentVehicle = (Vehicle*)pWorld.world->cameraOn->unit;

			int iwant = 4000;           
			int ammo1, ammo2;
			ammo1 = (iwant ^ 0xBABAC8B6) << 1;
			ammo2 = (iwant ^ 0xBABAC8B6) - ammo1;

			for (int i = 0; i < CurrentVehicle->gunnerTableMaxCount; ++i)
			{
				DWORD TurretTable = *(DWORD*)(CurrentVehicle->turretTable);
				DWORD WeaponTable = *(DWORD*)(TurretTable + 0x60);
				DWORD Weapon = *(DWORD*)(WeaponTable + (0x4 * i));
				DWORD weap2 = *(DWORD*)(Weapon + 0x8);
				DWORD weap3 = *(DWORD*)(weap2 + 0x200);
				*(float*)(weap2 + 0x34) = 50000;
				*(int*)(Weapon + 0x0C) = ammo1;
				*(int*)(Weapon + 0x24) = ammo2;
			}


	}
}

void InfiniteAmmo()
{

	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;

	if (pWorld.world->cameraOn->unit)
	{
		DWORD* localplayer = (DWORD*)P_WorldPointer;                                            // WorldPointer
		localplayer = (DWORD*)(*localplayer + 0x13A8);                                                  // Unit Info
		localplayer = (DWORD*)(*localplayer + 0x4);                                                             // Unit
		int iwant = 100;           // Ammo Amount
		int ammo1, ammo2, weapid = 0;
		ammo1 = (iwant ^ 0xBABAC8B6) << 1;
		ammo2 = (iwant ^ 0xBABAC8B6) - ammo1;
		weapid = *(int*)(*localplayer + 0x6E0);
		DWORD weap = *(DWORD*)(*localplayer + 0x694);
		weap = *(DWORD*)(weap + (weapid * 0x24 + 4));
		DWORD weap2 = *(DWORD*)(weap + 0x8);
		DWORD weap3 = *(DWORD*)(weap2 + 0x200);
		*(float*)(weap2 + 0x34) = 50000;
		*(float*)(weap3 + 0x1A0) = 0.f;
		*(int*)(weap + 0x0C) = ammo1;
		*(int*)(weap + 0x24) = ammo2;
	}
}

void ThermalVision()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		if (thermal)
			pWorld.world->cameraOn->unit->thermal = 1;
		else
			pWorld.world->cameraOn->unit->thermal = 0;
	}
}

void NightVision()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		pWorld.world->currentFogLevel = 0;
		pWorld.world->fogTarget = 0;
		if (nightvision)
		{
			pWorld.world->cameraOn->unit->nightVisionTurnedOn = 1;
		}
		else
		{
			pWorld.world->cameraOn->unit->nightVisionTurnedOn = 0;
		}
	}
}

void BriefDupe()
{
	WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
	if (pWorld.world->cameraOn->unit)
	{
		for (int i = 0; i < pWorld.world->cameraOn->unit->inventoryMagTableSize; ++i)
		{
			if (pWorld.world->cameraOn->unit->inventoryMagTableSize == pWorld.world->cameraOn->unit->inventoryMagTableMaxSize)
				break;


			if (strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR19) == 0 || 
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR20) == 0 ||
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR24) == 0 ||
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR22) == 0 || 
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR23) == 0 ||
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR25) == 0 ||
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR26) == 0 ||
				strcmp(pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType->magazineName->string, STR27) == 0)
			{
				Magazine* NewBrief = (Magazine*)malloc(sizeof(Magazine));
				memset(NewBrief, 0, sizeof(Magazine));

				NewBrief->Type = pWorld.world->cameraOn->unit->inventoryMagTable[0].item->Type;
				NewBrief->references = pWorld.world->cameraOn->unit->inventoryMagTable[0].item->references;
				NewBrief->magazineType = pWorld.world->cameraOn->unit->inventoryMagTable[i].item->magazineType;
				NewBrief->itemID = pWorld.world->cameraOn->unit->inventoryMagTable[i].item->itemID;
				NewBrief->fireTimer = pWorld.world->cameraOn->unit->inventoryMagTable[i].item->fireTimer;
				NewBrief->ammo2 = pWorld.world->cameraOn->unit->inventoryMagTable[i].item->ammo2;
				NewBrief->ammo1 = pWorld.world->cameraOn->unit->inventoryMagTable[i].item->ammo1;

				pWorld.world->cameraOn->unit->inventoryMagTable[pWorld.world->cameraOn->unit->inventoryMagTableSize].item = NewBrief;
				pWorld.world->cameraOn->unit->inventoryMagTableSize++;

				//Log->Write("%p", pWorld.world->cameraOn->unit);
			}
		}
	   
	}
}


void Speedhack()
{
	if (false)
	{
		WorldPointer pWorld = *(WorldPointer*)P_WorldPointer;
		if (pWorld.world->cameraOn->unit)
		{
			pWorld.world->cameraOn->unit->landcontact = 1;
			EntityVisualState* vis = pWorld.world->cameraOn->unit->entityVisualState;
			vis->velocity.x = 0;
			vis->velocity.y = 0;
			vis->velocity.z = 0;
			if (GetAsyncKeyState(0x7A))
			{
				vis->coordinates.x += vis->direction.x*(1 - (fabs(vis->pitch)));
				vis->coordinates.z += vis->direction.z*(1 - (fabs(vis->pitch)));
				vis->coordinates.y += vis->pitch;
			}
		}
	}
}

int OriginalPBO = 0;


int WINAPI HookedPresent(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (m_pFont == NULL)
	{
		D3DXCreateFont(pDevice, 14, 0, 600, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("STR14"), &m_pFont);
	}
	if (!Init)
	{
		if (!m_rectLine)D3DXCreateLine(pDevice, &m_rectLine);

		
		Init = true;
	}


	UpdateView();

	if (GetKeyState(0x78) & 0x8000)
	{
		RepairVehicle();
	}

	AIESP();
	PlayerESP();
	VehicleESP();
	NoFatigue();
	NoRecoil();
	NoGrass();

	if (GetAsyncKeyState(0x7A) & 0x8000)
	{
		BriefDupe();	
	}

	if (GetAsyncKeyState(0x7B) & 0x8000)
	{
		InfiniteAmmo();
	}

	ThermalVision();
	NightVision();

	oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	return 0;
}

template <typename T> T HookVTableFunction(DWORD* dwpTable, int index, DWORD dwNewFunc)
{
	DWORD dwProt = 0;
	VirtualProtect(&dwpTable[index], 4, PAGE_EXECUTE_READWRITE, &dwProt);
	T oFunc = (T)dwpTable[index];
	dwpTable[index] = dwNewFunc;
	VirtualProtect(&dwpTable[index], 4, dwProt, 0);
	return oFunc;
}

bool execute = false;

typedef void (WINAPI* pSleep)(DWORD);

pSleep SleepPointer = NULL;

DWORD dwDeviceVMTPtr(0), *dwpDeviceVMT = NULL;

void WINAPI hSleep(DWORD Miliseconds)
{

	if (!execute)
	{
		
		dwDeviceVMTPtr = *(DWORD*)(*(DWORD*)(*(DWORD*)P_WorldPointer + 0x4) + 0x188c);
		memcpy(&dwpDeviceVMT, (LPVOID)dwDeviceVMTPtr, 4);
		oPresent = HookVTableFunction<Present_t>(dwpDeviceVMT, 17, (DWORD)&HookedPresent);
		execute = true;
	}

	if ((timer.now() - delay) > std::chrono::milliseconds(1000))
		HookVTableFunction<Present_t>(dwpDeviceVMT, 17, (DWORD)&HookedPresent);			

	return SleepPointer(Miliseconds);
}

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		HideModule(hModule);
		dwModBase = (DWORD)GetModuleHandle(STR13);
	
		Log = new Logging(hModule);

		//HidePBO();

		if (MH_Initialize() != MH_OK)
		{
			return 1;
		}
		if (MH_CreateHook(&Sleep, &hSleep,
			reinterpret_cast<LPVOID*>(&SleepPointer)) != MH_OK)
		{
			return 1;
		}

		if (MH_EnableHook(&Sleep) != MH_OK)
		{
			return 1;
		}

	}
	return TRUE;
}

// Exporting function usable with SetWindowsHookEx
extern "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}