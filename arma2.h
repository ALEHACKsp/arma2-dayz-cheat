#include <windows.h>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

class ArmaString
{
public:
	__int32 referenceCount; //0x0000  
	__int32 Length; //0x0004  
	char string[150]; //0x0008  
};

class scriptValue
{
public:
	char _0x0000[12];
	ArmaString *value;
};

class varEntry
{
public:
	DWORD vTable;
	ArmaString* name;
	char _0x0008[4];
	scriptValue* value;
};

class EntryArray
{
public:
	varEntry* firstEntry; //0x0000
};

class VarTable
{
public:
	char _0x0000[4];
	EntryArray* eArray; //0x0004
	char _0x000C[22];
	BYTE number;
};

class EventTable
{
public:
	char _0x0000[8];
	DWORD* eventText; //0x0008  

};//Size=0x000C  

class EventHandler
{
public:
	char _0x0000[8];
	EventTable* eventTable; //0x0008  
	__int32 eventTableSize; //0x000C  
	__int32 eventTableMaxSize; //0x0010  

};//Size=0x0014 

class EntityVisualState
{
public:
	char _0x0000[4];
	D3DXVECTOR3 dimension; //0x0004  
	D3DXVECTOR3 rotation1; //0x0010  
	D3DXVECTOR3 direction; //0x001C  
	D3DXVECTOR3 coordinates; //0x0028  
	char _0x0034[20];
	D3DXVECTOR3 velocity; //0x0048  
	float angularVelocity; //0x0054  
	float zVelocity2; //0x0058  
	float Speed; //0x005C  
	D3DXVECTOR3 acceleration; //0x0060  
	char _0x006C[16];
	D3DXVECTOR3 direction2; //0x007C  
	D3DXVECTOR3 rotation2; //0x0088  
	D3DXVECTOR3 direction3; //0x0094  
	char _0x00A0[12];
	float fuelLevel; //0x00AC  
	char _0x00B0[92];
	D3DXVECTOR3 headCoordinates; //0x010C  
	D3DXVECTOR3 torsoCoordinates; //0x0118  
	char _0x0124[0xBC];
	float pitch; //0x01E0
};//Size=0x02E4 

class CfgVehicle
{
public:
	char _0x0000[48];
	ArmaString* entityName; //0x0030  
	char _0x0034[56];
	ArmaString* objectType; //0x006C  
	char _0x0070[60];
	DWORD* partHit; //0x00AC  
	__int32 partHitCount; //0x00B0  
	char _0x00B4[1096];
	DWORD* initializationScript; //0x04FC  
	char _0x0500[76];
	DWORD* local; //0x054C  
	char _0x0550[157];
	BYTE enableWatch; //0x05ED  
	char _0x05EE[2];
	float armor; //0x05F0  
	char _0x05F4[12];
	float fuelTankCap; //0x0600  
	float damageResistance; //0x0604  
	char _0x0608[4];
	float sensitivity; //0x060C  
	char _0x0610[4];
	float brakeDistance; //0x0614  
	float precision; //0x0618  
	char _0x061C[58];
	BYTE nightVision; //0x0656  
	char _0x0657[117];
	__int32 transportFuel; //0x06CC  
	__int32 transportRepair; //0x06D0  
	__int32 transportAmmo; //0x06D4  
	__int32 transportMaxWeapons; //0x06D8  
	__int32 transportMaxMagazines; //0x06DC  
	__int32 transportMaxBackpacks; //0x06E0  
	char _0x06E4[68];
	float maxSpeed; //0x0728  
	float limitedSpeedCoef; //0x072C  
	char _0x0730[724];

};//Size=0x0A04 

class WeaponModeType
{
public:
	char _0x0000[4];
	__int32 scope; //0x0004  
	char _0x0008[8];
	__int32 roundsDeducted; //0x0010  
	__int32 burst; //0x0014  
	char _0x0018[16];
	float fireRate; //0x0028  
	float dispersion; //0x002C  
	ArmaString* recoil; //0x0030  
	ArmaString* recoilProne; //0x0034  
	char _0x0038[10];
	BYTE autoFire; //0x0042  
	char _0x0043[1];

};//Size=0x0044 

class ScoreboardEntry
{
public:
	char _0x0000[4];
	__int32 scoreboardID; //0x0004  
	char _0x0008[0x2C];
	ArmaString* uid; //0x0034  
	char _0x0038[0x70];
	ArmaString* playerName; //0x00A8  

};//Size=0x008C 

class Scoreboard
{
public:
	char _0x0000[24];
	ScoreboardEntry* scoreboardTable; //0x0018  
	__int32 scoreboardTableCount; //0x001C  
	__int32 scoreboardTableMaxCount; //0x0020  

};//Size=0x0024 

class NetworkManager
{
public:
	char _0x0000[36];
	Scoreboard* scoreboard; //0x0024  
	char _0x0028[1176];

};//Size=0x04C0 

class Ammunition
{
public:
	char _0x0000[48];
	ArmaString* ammoName; //0x0030  
	char _0x0034[56];
	ArmaString* bulletModelString; //0x006C  
	char _0x0070[208];
	float directDamage; //0x0140  
	float indirectDamage; //0x0144  
	float indirectRange; //0x0148  
	float maxControlRange; //0x014C  
	float maneuverability; //0x0150  
	float trackOversteer; //0x0154  
	float trackLead; //0x0158  
	float initTime; //0x015C  
	float thrustTime; //0x0160  
	float thrust; //0x0164  
	float sideAirFriction; //0x0168  
	float explosionTime; //0x016C  
	float fuseDistance; //0x0170  
	float cost; //0x0174  
	float maxSpeed; //0x0178  
	float simulationStep; //0x017C  
	char _0x0180[8];
	float explosive; //0x0188  
	float caliber; //0x018C  
	float deflecting; //0x0190  
	float deflectionSlowDown; //0x0194  
	float timeToLive; //0x0198  
	float airFriction; //0x019C  
	float coefGravity; //0x01A0  
	float typicalSpeed; //0x01A4  
	char _0x01A8[24];
	__int32 tracerColor; //0x01C0  
	__int32 tracerColor2; //0x01C4  
	float audibleFire; //0x01C8  
	float visibleFire; //0x01CC  
	float visibleFireTime; //0x01D0  
	char _0x01D4[292];
	float tracerScale; //0x02F8  
	float tracerStartTime; //0x02FC  
	float tracerEndTime; //0x0300  
	BYTE nvgOnly; //0x0304  
	char _0x0305[307];
	float fireTimeFloor; //0x0438  
	char _0x043C[132];

};//Size=0x04C0 

class MagazineType
{
public:
	char _0x0000[20];
	ArmaString* magazineName; //0x0014  
	char _0x0018[4];
	ArmaString* magazineModel; //0x001C  
	ArmaString* objectType; //0x0020  
	char _0x0024[8];
	__int32 magazineCapacity; //0x002C  
	char _0x0030[4];
	float muzzleVelocity; //0x0034  
	char _0x0038[24];
	ArmaString* action; //0x0050  
	char _0x0054[428];
	Ammunition* ammunition; //0x0200  
	__int32 tracersEvery; //0x0204  
	__int32 lastRoundsTracer; //0x0208  
	char _0x020C[116];

};//Size=0x0280 

class Magazine
{
public:
	char _0x0000[4];
	__int32 references; //0x0004  
	MagazineType* magazineType; //0x0008  
	__int32 ammo1; //0x000C  
	char _0x0010[4];
	float fireTimer; //0x0014  
	char _0x0018[12];
	__int32 ammo2; //0x0024  
	__int32 Type; //0x0028  
	__int32 itemID; //0x002C  

};//Size=0x0030 

struct MagTable
{
	Magazine* mag;
	int count;
};

class Weapon
{
public:
	char _0x0000[4];
	Magazine* magazine; //0x0004  
	DWORD* weaponType; //0x0008  
	DWORD* muzzle; //0x000C  
	WeaponModeType* weaponModeType; //0x0010  
	char _0x0014[16];
};

class Entity
{
public:
	char _0x0000[24];
	EntityVisualState* entityVisualState; //0x0018  
	char _0x001C[32];
	CfgVehicle* cfgVehicle; //0x003C  
	char _0x0040[476];
	DWORD* entityInventory; //0x021C  

};//Size=0x0220 

class InventoryMag
{
public:
	Magazine* item; //0x0000  

};//Size=0x0004 

class Unit
{
public:
	char _0x0000[24];
	EntityVisualState* entityVisualState; //0x0018  
	char _0x001C[32];
	CfgVehicle* cfgVehicle; //0x003C  
	char _0x0040[128];
	float* playerDamage; //0x00C0  
	__int32 damagePartCount; //0x00C4  
	char _0x00C8[148];
	__int32 side; //0x015C  
	char _0x0160[1];
	BYTE landcontact;
	char _0x0162[2];
	__int8 hasName; //0x0164  
	char _0x0165[167];
	BYTE isDead; //0x020C  
	char _0x020D[6];
	BYTE allowDamage; //0x0213  
	char _0x0214[152];
	EventHandler eventHandlers[34]; //0x02AC  
	char _0x0554[108];
	DWORD* playerVariables; //0x05C0  // <-
	char _0x05C4[148];
	float audibleFire; //0x0658  
	float visibleFire; //0x065C  
	float visibleFireTime; //0x0660  
	char _0x0664[36];
	DWORD* itemTable; //0x0688  // <-
	__int32 itemTableSize; //0x068C  
	__int32 itemTableMaxSize; //0x0690  
	Weapon* weaponTable; //0x0694  
	__int32 weaponTableSize; //0x0698  
	__int32 weaponTableSize2; //0x069C  
	InventoryMag* inventoryMagTable; //0x06A0  <-
	__int32 inventoryMagTableSize; //0x06A4  
	__int32 inventoryMagTableMaxSize; //0x06A8  
	Entity* backpack; //0x06AC  
	char _0x06B0[36];
	float primaryHeat; //0x06D4  
	float primaryHeat2; //0x06D8  
	float secondaryHeat; //0x06DC  
	__int32 weaponID; //0x06E0  
	char _0x06E4[668];
	D3DXVECTOR3 prevPos[10]; //0x0980  
	char _0x09F8[32];
	DWORD* aiBrain; //0x0A18  
	char _0x0A1C[124];
	__int32 vehicleLocked; //0x0A98  
	char _0x0A9C[0x1C];
	D3DXVECTOR3 markerPos; //0x0AB8
	int PAD;
	__int32 playerID; //0x0AC8  
	char _0x0ACC[4];
	float innerReticleRelativeYaw; //0x0AD0  
	float innerReticlePitch; //0x0AD4  
	char _0x0AD8[8];
	float outerReticlePitch; //0x0AE0  
	char _0x0AE4[8];
	float innerReticleWanderY; //0x0AEC  
	float innerReticleWanderX; //0x0AF0  
	char _0x0AF4[12];
	float headDirectionX; //0x0B00  
	float headDirectionX2; //0x0B04  
	char _0x0B08[12];
	float headDirectionY; //0x0B14  
	float headDirectionY2; //0x0B18  
	char _0x0B1C[24];
	float headDirectionAbsoluteY; //0x0B34  
	char _0x0B38[12];
	float zoom; //0x0B44  
	char _0x0B48[28];
	float lean; //0x0B64  
	char _0x0B68[88];
	__int32 hideBody; //0x0BC0  
	char _0x0BC4[81];
	BYTE nightVisionTurnedOn; //0x0C15  
	BYTE nightVisionEnabled; //0x0C16  
	BYTE binocularsEnabled; //0x0C17  
	BYTE thermal; //0x0C18  
	char _0x0C19[1];
	BYTE zoomedOut; //0x0C1A  
	BYTE zoomedIn; //0x0C1B  
	char _0x0C1C[12];
	float customRecoilCoef; //0x0C28  
	char _0x0C2C[20];
	float breath; //0x0C40  
	float fatigue; //0x0C44  
	char _0x0C48[28];
	float reticleWanderX; //0x0C64  
	float reticleWanderY; //0x0C68  
	char _0x0C6C[1280];
	float velocityX; //0x116C  
	float velocityY; //0x1170  
	float velocityZ; //0x1174  

	void Repair()
	{
		if (this)
		{
			this->entityVisualState->fuelLevel = 500000.f;

			for (int i = 0; i < this->damagePartCount; ++i)
				this->playerDamage[i] = 0.f;

		}
	}

};//Size=0x1178 

class UnitInfo
{
public:
	char _0x0000[4];
	Unit* unit; //0x0004  
	char _0x0008[44];

};//Size=0x0034 

class Vehicle
{
public:
	char _0x0000[24];
	EntityVisualState* coordinates; //0x0018  
	char _0x001C[32];
	CfgVehicle* cfgVehicle; //0x003C  
	char _0x0040[128];
	DWORD* damageParts; //0x00C0  
	__int32 damagePartsCount; //0x00C4  
	char _0x00C8[1272];
	DWORD* unitVariables; //0x05C0  
	char _0x05C4[1260];
	Unit* driver; //0x0AB0  
	char _0x0AB4[24];
	BYTE N060066D2; //0x0ACC  
	char _0x0ACD[2];
	BYTE N0426D661; //0x0ACF  
	char _0x0AD0[44];
	DWORD* passengers; //0x0AFC  
	__int32 maxPassengers; //0x0B00  
	char _0x0B04[88];
	DWORD* turretTable; //0x0B5C  
	__int32 gunnerTableCount; //0x0B60  
	__int32 gunnerTableMaxCount; //0x0B64  

};//Size=0x0B68 

class EntityTable
{
public:
	UnitInfo* unitInfo; //0x0000  
	Entity* e; //0x0004
	float xCoord; //0x0008  
	float yCoord; //0x000C  
	float zCoord; //0x0010  
	char _0x0014[32];

};//Size=0x0034 

class EntityTablePointer
{
public:
	EntityTable* entityTable; //0x0000  
	__int32 objectTableSize; //0x0004  

};//Size=0x0008 

class EntityList
{
public:
	Entity* entity; //0x0000  

};//Size=0x0004 

class EntitiesDistributed
{
public:
	char _0x0000[8];
	EntityList* table1; //0x0008  
	__int32 table1Size; //0x000C  
	char _0x0010[160];
	EntityList* table2; //0x00B0  
	__int32 table2Size; //0x00B4  
	char _0x00B8[160];
	EntityList* table3; //0x0158  
	__int32 table3Size; //0x015C  
	char _0x0160[160];
	EntityList* table4; //0x0200  
	__int32 table4Size; //0x0204  
	char _0x0208[120];

};//Size=0x0280 

class World
{
public:
	char _0x0000[8];
	DWORD* inGameUI; //0x0008  
	char _0x000C[1520];
	EntityTablePointer* entityTablePointer; //0x05FC   
	DWORD* variableTableInfo; //0x0600  
	char _0x0604[428];
	__int32 gameMode; //0x07B0  
	char _0x07B4[4];
	float speedMultiplier; //0x07B8  
	char _0x07BC[0x16C];
	EntitiesDistributed* table1; //0x0928  
	char _0x0880[0x2A0];
	EntitiesDistributed* table2; //0x0BCC  
	char _0x0B24[0x348];
	EntitiesDistributed* table3; //0x0F18  
	char _0x0DC8[1164];
	UnitInfo* cameraOn; //0x13A8  
	char _0x13AC[4];
	UnitInfo* cplayerOn; //0x13B0  
	UnitInfo* realPlayer; //0x13B4  
	char _0x13B8[48];
	float actualOvercast; //0x13E8  
	float wantedOvercast; //0x13EC  
	__int32 nextWeatherChange; //0x13F0  
	float currentFogLevel; //0x13F4  
	float fogTarget; //0x13F8  
	char _0x13FC[32];
	__int32 weatherTime; //0x141C  
	char _0x1420[8];
	BYTE playerManual; //0x1428  
	BYTE playerSuspended; //0x1429  
	char _0x142A[30];
	__int32 N0D09AD19; //0x1448  
	char _0x144C[92];
	DWORD* currentCampaign; //0x14A8  
	char _0x14AC[4];
	__int32 N0D09B79F; //0x14B0  
	char _0x14B4[52];
	float viewDistanceHard; //0x14E8  
	float viewDistanceMin; //0x14EC  
	float grass; //0x14F0  
	char _0x14F4[36];
	__int32 initTableCount; //0x1518  
	__int32 initTableMaxCount; //0x151C  
	char _0x1520[4];

};//Size=0x1524 

class WorldPointer
{
public:
	World* world; //0x0000  
	char _0x0004[2556];

};//Size=0x0A00 
#pragma once
