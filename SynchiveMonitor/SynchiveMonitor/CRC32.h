#pragma once

ref class CRC32
{
public:
	property unsigned long value;

	CRC32();
	String^ computeHash(String^ filePath);
	String^ computeHash(BinaryReader ^ br);
	String^ ToString() override;
private:
	array<unsigned int>^ table;
	void updateCRC32(array<Byte>^ buffer, int count);
};

