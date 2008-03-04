#include "config.h"

int P2PSEL0::CGlobalInformation::m_iChatStream = -1;
int P2PSEL0::CGlobalInformation::m_iGroupEventProtocolStream = -1;
double P2PSEL0::CGlobalInformation::m_dTimeoutValue = 20.0;
double P2PSEL0::CGlobalInformation::m_dGroupTimeoutValue = 5.0;
double P2PSEL0::CGlobalInformation::m_dBannedTimeout = 5.0;

P2PSEL0::EGroupType P2PSEL0::GetGroupType(TGroupID id)
{
  if (id == 0)
    return GROUP_LIMBO_TYPE;
  if (id < 1024)
    return GROUP_SOCIAL_TYPE;
  else
    return GROUP_ACTION_TYPE;
}

