#pragma once

#include "config.h"
#include <vector>

template< class T > 
  class CIdGenerator
  {
  public:
    ///Constructor
    CIdGenerator(T tStart = (INVALID_ID + 1), T tEnd = 4095): m_iActual(INVALID_ID), m_tStart(tStart)
    {
      m_oAvailability.resize((tEnd - max<T>(INVALID_ID + 1, tStart)) + 1, false);
    }

    T GetID() 
    { 
      for (T i = m_iActual + 1; i != m_iActual; i = (i + 1) % (T)m_oAvailability.size())
      {
        if (!m_oAvailability[i])
        {
          m_oAvailability[i] = true;
          m_iActual = i;
          return i + m_tStart;
        }
      }
      return (T)m_oAvailability.size() + m_tStart;
    }

    T GetLastCreatedID()
    {
      return m_iActual + m_tStart;
    }

    const bool FreeID(const T id) 
    {
      const T vid = id - m_tStart;
      if ( (id < m_tStart) || (vid >= m_oAvailability.size()) )
        return false;
      if (!m_oAvailability[vid])
        return false;
      m_oAvailability[vid] = false;
      return true;
    }

    const bool IsValid(T id) 
    { 
      const T vid = id - m_tStart;
      if ( (vid <= INVALID_ID) || (vid >= m_oAvailability.size()) )
        return false;
      return m_oAvailability[vid];
    }

    const T GetSize() { return (T)m_oAvailability.size(); }
    
  private:
    /// Disponibilidade
    std::vector< bool > m_oAvailability;
    /// Index
    T m_iActual;
    /// Start index
    T m_tStart;
  };

