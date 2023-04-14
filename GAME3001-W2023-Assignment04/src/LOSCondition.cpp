#include  "LOSCondition.h"

LOSCondition::LOSCondition(const bool hasLOS)
{
	m_name = "Close Combat Condition";
	SetLOS(hasLOS);
}

LOSCondition::~LOSCondition()
= default;

void LOSCondition::SetLOS(const bool state)
{
	m_hasLOS = state;
}

bool LOSCondition::Condition()
{
	return m_hasLOS;
}