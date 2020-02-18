
class PBONAME
{
public:
	char _0x0000[4];
	__int32 size; //0x0004 
	char string[255]; //0x0008 
	char _0x000C[52];

	std::string GetPBO()
	{
		std::string t = std::string(this->string);
		t.resize(this->size);
		return t;
	}
};//Size=0x0080

class PBOINFO
{
public:
	char _0x0000[16];
	PBONAME* Name; //0x0010 
	char _0x0014[44];

};//Size=0x0040

class PBOLIST
{
public:
	PBOINFO* Info; //0x0000 
	char _0x0004[1084];

};//Size=0x0440





class PBO_BANK
{
public:
	DWORD* List; //0x0000 
	__int32 Count; //0x0004 
	char _0x0008[56];

};//Size=0x0040

