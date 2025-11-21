/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2025
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Benjamin Huh <buh07@github>
 */

#include "ble-discovery-header.h"
#include "ns3/log.h"
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BleDiscoveryHeader");

NS_OBJECT_ENSURE_REGISTERED (BleDiscoveryHeader);

BleDiscoveryHeader::BleDiscoveryHeader ()
  : m_messageType (DISCOVERY),
    m_senderId (0),
    m_ttl (10),
    m_gpsLocation (Vector (0, 0, 0)),
    m_gpsAvailable (false),
    m_classId (0),
    m_pdsf (0),
    m_score (0.0),
    m_hash (0)
{
  NS_LOG_FUNCTION (this);
}

BleDiscoveryHeader::~BleDiscoveryHeader ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
BleDiscoveryHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BleDiscoveryHeader")
    .SetParent<Header> ()
    .SetGroupName ("BleMeshDiscovery")
    .AddConstructor<BleDiscoveryHeader> ()
  ;
  return tid;
}

TypeId
BleDiscoveryHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
BleDiscoveryHeader::Print (std::ostream &os) const
{
  os << "BleDiscoveryHeader: ";
  os << "Type=" << (m_messageType == DISCOVERY ? "DISCOVERY" : "ELECTION") << ", ";
  os << "ID=" << m_senderId << ", ";
  os << "TTL=" << (uint32_t)m_ttl << ", ";
  os << "PSF=[";
  for (size_t i = 0; i < m_pathSoFar.size (); ++i)
    {
      os << m_pathSoFar[i];
      if (i < m_pathSoFar.size () - 1)
        os << ",";
    }
  os << "], ";
  os << "GPS=" << (m_gpsAvailable ? "available" : "unavailable");

  if (m_messageType == ELECTION_ANNOUNCEMENT)
    {
      os << ", ClassID=" << m_classId;
      os << ", PDSF=" << m_pdsf;
      os << ", Score=" << m_score;
      os << ", Hash=" << m_hash;
    }
}

uint32_t
BleDiscoveryHeader::GetSerializedSize (void) const
{
  // TODO: Implement proper serialization size calculation
  // Message Type (1 byte) + Sender ID (4 bytes) + TTL (1 byte)
  uint32_t size = 1 + 4 + 1;

  // PSF: length (2 bytes) + node IDs (4 bytes each)
  size += 2 + (m_pathSoFar.size () * 4);

  // GPS: availability flag (1 byte) + coordinates (3 * 8 bytes if available)
  size += 1;
  if (m_gpsAvailable)
    {
      size += 3 * 8; // 3 doubles for x, y, z
    }

  // Election announcement fields (only if type is ELECTION_ANNOUNCEMENT)
  if (m_messageType == ELECTION_ANNOUNCEMENT)
    {
      size += 2; // Class ID (2 bytes)
      size += 4; // PDSF (4 bytes)
      size += 8; // Score (8 bytes, double)
      size += 4; // Hash (4 bytes)
    }

  return size;
}

void
BleDiscoveryHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  // TODO: Implement serialization
  // Write message type
  start.WriteU8 ((uint8_t)m_messageType);

  // Write sender ID
  start.WriteHtonU32 (m_senderId);

  // Write TTL
  start.WriteU8 (m_ttl);

  // Write Path So Far
  start.WriteHtonU16 ((uint16_t)m_pathSoFar.size ());
  for (size_t i = 0; i < m_pathSoFar.size (); ++i)
    {
      start.WriteHtonU32 (m_pathSoFar[i]);
    }

  // Write GPS availability
  start.WriteU8 (m_gpsAvailable ? 1 : 0);
  if (m_gpsAvailable)
    {
      // Write GPS coordinates as doubles (x, y, z)
      start.WriteHtonU64 (*reinterpret_cast<const uint64_t*>(&m_gpsLocation.x));
      start.WriteHtonU64 (*reinterpret_cast<const uint64_t*>(&m_gpsLocation.y));
      start.WriteHtonU64 (*reinterpret_cast<const uint64_t*>(&m_gpsLocation.z));
    }

  // Write election announcement fields if applicable
  if (m_messageType == ELECTION_ANNOUNCEMENT)
    {
      start.WriteHtonU16 (m_classId);
      start.WriteHtonU32 (m_pdsf);
      start.WriteHtonU64 (*reinterpret_cast<const uint64_t*>(&m_score));
      start.WriteHtonU32 (m_hash);
    }
}

uint32_t
BleDiscoveryHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);

  // TODO: Implement deserialization
  Buffer::Iterator i = start;

  // Read message type
  m_messageType = (MessageType)i.ReadU8 ();

  // Read sender ID
  m_senderId = i.ReadNtohU32 ();

  // Read TTL
  m_ttl = i.ReadU8 ();

  // Read Path So Far
  uint16_t psfLength = i.ReadNtohU16 ();
  m_pathSoFar.clear ();
  for (uint16_t j = 0; j < psfLength; ++j)
    {
      m_pathSoFar.push_back (i.ReadNtohU32 ());
    }

  // Read GPS availability
  m_gpsAvailable = (i.ReadU8 () == 1);
  if (m_gpsAvailable)
    {
      uint64_t xBits = i.ReadNtohU64 ();
      uint64_t yBits = i.ReadNtohU64 ();
      uint64_t zBits = i.ReadNtohU64 ();
      m_gpsLocation.x = *reinterpret_cast<double*>(&xBits);
      m_gpsLocation.y = *reinterpret_cast<double*>(&yBits);
      m_gpsLocation.z = *reinterpret_cast<double*>(&zBits);
    }

  // Read election announcement fields if applicable
  if (m_messageType == ELECTION_ANNOUNCEMENT)
    {
      m_classId = i.ReadNtohU16 ();
      m_pdsf = i.ReadNtohU32 ();
      uint64_t scoreBits = i.ReadNtohU64 ();
      m_score = *reinterpret_cast<double*>(&scoreBits);
      m_hash = i.ReadNtohU32 ();
    }

  return GetSerializedSize ();
}

// ===== Setters and Getters =====

void
BleDiscoveryHeader::SetMessageType (MessageType type)
{
  m_messageType = type;
}

BleDiscoveryHeader::MessageType
BleDiscoveryHeader::GetMessageType (void) const
{
  return m_messageType;
}

void
BleDiscoveryHeader::SetSenderId (uint32_t id)
{
  m_senderId = id;
}

uint32_t
BleDiscoveryHeader::GetSenderId (void) const
{
  return m_senderId;
}

void
BleDiscoveryHeader::SetTtl (uint8_t ttl)
{
  m_ttl = ttl;
}

uint8_t
BleDiscoveryHeader::GetTtl (void) const
{
  return m_ttl;
}

bool
BleDiscoveryHeader::DecrementTtl (void)
{
  if (m_ttl > 0)
    {
      m_ttl--;
      return true;
    }
  return false;
}

void
BleDiscoveryHeader::SetPathSoFar (const std::vector<uint32_t> &psf)
{
  m_pathSoFar = psf;
}

std::vector<uint32_t>
BleDiscoveryHeader::GetPathSoFar (void) const
{
  return m_pathSoFar;
}

void
BleDiscoveryHeader::AddToPath (uint32_t nodeId)
{
  m_pathSoFar.push_back (nodeId);
}

bool
BleDiscoveryHeader::IsInPath (uint32_t nodeId) const
{
  return std::find (m_pathSoFar.begin (), m_pathSoFar.end (), nodeId) != m_pathSoFar.end ();
}

void
BleDiscoveryHeader::SetGpsLocation (Vector position)
{
  m_gpsLocation = position;
}

Vector
BleDiscoveryHeader::GetGpsLocation (void) const
{
  return m_gpsLocation;
}

void
BleDiscoveryHeader::SetGpsAvailable (bool available)
{
  m_gpsAvailable = available;
}

bool
BleDiscoveryHeader::IsGpsAvailable (void) const
{
  return m_gpsAvailable;
}

// ===== Election Announcement Fields =====

void
BleDiscoveryHeader::SetClassId (uint16_t classId)
{
  m_classId = classId;
}

uint16_t
BleDiscoveryHeader::GetClassId (void) const
{
  return m_classId;
}

void
BleDiscoveryHeader::SetPdsf (uint32_t pdsf)
{
  m_pdsf = pdsf;
}

uint32_t
BleDiscoveryHeader::GetPdsf (void) const
{
  return m_pdsf;
}

void
BleDiscoveryHeader::SetScore (double score)
{
  m_score = score;
}

double
BleDiscoveryHeader::GetScore (void) const
{
  return m_score;
}

void
BleDiscoveryHeader::SetHash (uint32_t hash)
{
  m_hash = hash;
}

uint32_t
BleDiscoveryHeader::GetHash (void) const
{
  return m_hash;
}

} // namespace ns3
