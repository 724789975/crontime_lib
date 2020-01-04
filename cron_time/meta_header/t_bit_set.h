#ifndef __TBitSet_h__
#define __TBitSet_h__

#include <string.h>
#include <iostream>

class BitSetInterface
{
public:
	virtual bool					SetBit(unsigned int e, bool bValue) = 0;
	virtual bool					GetBit(unsigned int e) const = 0;
	virtual unsigned int			GetCount() const = 0;
	virtual void					Reset() = 0;
	virtual void					DumpInfo(std::ostream& refStream) const = 0;
};

//这个类型的对象根本不会被创建成功
#define EnumBitSet(EnumName) \
template<EnumName>\
class T##EnumName##BitSet\
{\
private:\
	T##EnumName##BitSet();\
};\
template<>\
class T##EnumName##BitSet<EnumName##Count> : public BitSetInterface \
{\
public:\
	T##EnumName##BitSet(){memset(m_pData, 0, sizeof(m_pData));}\
	virtual ~T##EnumName##BitSet(){}\
	bool SetBit(unsigned int e, bool bValue)\
	{\
		if (e >= EnumName##Count)\
		{\
			return false;\
		}\
		if (bValue)\
		{\
			m_pData[e / 8] |= (1 << (e % 8));\
		}\
		else\
		{\
			m_pData[e / 8] &= ~(1 << (e % 8));\
		}\
		return true;\
	}\
	bool GetBit(unsigned int e)const \
	{\
		if (e >= EnumName##Count)\
		{\
			return false;\
		}\
		return (m_pData[e / 8] & (1 << (e % 8))) != 0;\
	}\
	unsigned int GetCount() const \
	{\
		return (unsigned int)EnumName##Count;\
	}\
	void Reset()\
	{\
		memset(m_pData, 0, sizeof(m_pData));\
	}\
	void DumpInfo(std::ostream& refStream) const\
	{\
		for (unsigned int i = 0; i < EnumName##Count; ++i)\
		{\
			refStream << i << " : " << GetBit(i) << ", ";\
		}\
		refStream << "\n";\
	}\
private:\
	char m_pData[EnumName##Count / 8 + 1];\
};\
typedef T##EnumName##BitSet<EnumName##Count> EnumName##BitSet;

#define BeginEnumBitSet(EnumName) \
enum EnumName

#define EndEnumBitSet(EnumName) \
EnumName##Count,

#define EndEnumBitSetWithCount(EnumName, NUM) \
EnumName##Count = NUM,


#endif // !__TBitSet_h__
