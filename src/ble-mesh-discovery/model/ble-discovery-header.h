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
 *
 * BLE Mesh Discovery Protocol Header
 * Based on: "Clusterhead & BLE Mesh discovery process" by jason.peng (November 2025)
 */

#ifndef BLE_DISCOVERY_HEADER_H
#define BLE_DISCOVERY_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/vector.h"
#include <vector>
#include <stdint.h>

namespace ns3 {

/**
 * \ingroup ble-mesh-discovery
 * \brief BLE Discovery Message Header
 *
 * This header implements the discovery message format for the BLE mesh protocol:
 * - ID: Unique identifier of the message sender
 * - TTL: Time To Live - number of hops remaining before message is discontinued
 * - PSF: Path So Far - sequence of node IDs the message has traveled through
 * - LHGPS: Last Heard GPS location of the message sender
 *
 * The header also supports election announcement messages with additional fields:
 * - Class ID: Clusterhead class identifier
 * - PDSF: Predicted Devices So Far
 * - Score: Clusterhead candidacy score
 * - Hash: FDMA/TDMA hash function
 */
class BleDiscoveryHeader : public Header
{
public:
  /**
   * \brief Message type enumeration
   */
  enum MessageType
  {
    DISCOVERY = 0,           //!< Basic discovery message
    ELECTION_ANNOUNCEMENT = 1 //!< Clusterhead election announcement
  };

  /**
   * \brief Constructor
   */
  BleDiscoveryHeader ();

  /**
   * \brief Destructor
   */
  virtual ~BleDiscoveryHeader ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Get the type ID for this instance
   * \return the instance TypeId
   */
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * \brief Print the header to an output stream
   * \param os output stream
   */
  virtual void Print (std::ostream &os) const;

  /**
   * \brief Get the serialized size of the header
   * \return the serialized size
   */
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * \brief Serialize the header to a buffer
   * \param start the buffer iterator
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the header from a buffer
   * \param start the buffer iterator
   * \return the number of bytes read
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  // ===== Setters and Getters =====

  /**
   * \brief Set the message type
   * \param type the message type
   */
  void SetMessageType (MessageType type);

  /**
   * \brief Get the message type
   * \return the message type
   */
  MessageType GetMessageType (void) const;

  /**
   * \brief Set the sender ID
   * \param id the unique identifier of the message sender
   */
  void SetSenderId (uint32_t id);

  /**
   * \brief Get the sender ID
   * \return the sender ID
   */
  uint32_t GetSenderId (void) const;

  /**
   * \brief Set the Time To Live (TTL)
   * \param ttl the number of hops remaining
   */
  void SetTtl (uint8_t ttl);

  /**
   * \brief Get the Time To Live (TTL)
   * \return the TTL value
   */
  uint8_t GetTtl (void) const;

  /**
   * \brief Decrement the TTL by 1
   * \return true if TTL > 0 after decrement, false otherwise
   */
  bool DecrementTtl (void);

  /**
   * \brief Set the Path So Far (PSF)
   * \param psf vector of node IDs representing the path
   */
  void SetPathSoFar (const std::vector<uint32_t> &psf);

  /**
   * \brief Get the Path So Far (PSF)
   * \return the path vector
   */
  std::vector<uint32_t> GetPathSoFar (void) const;

  /**
   * \brief Add a node ID to the Path So Far
   * \param nodeId the node ID to add
   */
  void AddToPath (uint32_t nodeId);

  /**
   * \brief Check if a node is already in the path (for loop detection)
   * \param nodeId the node ID to check
   * \return true if node is in path, false otherwise
   */
  bool IsInPath (uint32_t nodeId) const;

  /**
   * \brief Set GPS coordinates (Last Heard GPS)
   * \param position the GPS coordinates as a Vector (x, y, z)
   */
  void SetGpsLocation (Vector position);

  /**
   * \brief Get GPS coordinates
   * \return the GPS position vector
   */
  Vector GetGpsLocation (void) const;

  /**
   * \brief Set GPS availability flag
   * \param available true if GPS is available, false otherwise
   */
  void SetGpsAvailable (bool available);

  /**
   * \brief Check if GPS is available
   * \return true if GPS available, false otherwise
   */
  bool IsGpsAvailable (void) const;

  // ===== Election Announcement Fields =====

  /**
   * \brief Set the clusterhead class ID
   * \param classId the class ID
   */
  void SetClassId (uint16_t classId);

  /**
   * \brief Get the clusterhead class ID
   * \return the class ID
   */
  uint16_t GetClassId (void) const;

  /**
   * \brief Set Predicted Devices So Far (PDSF)
   * \param pdsf the predicted device count
   */
  void SetPdsf (uint32_t pdsf);

  /**
   * \brief Get Predicted Devices So Far (PDSF)
   * \return the PDSF value
   */
  uint32_t GetPdsf (void) const;

  /**
   * \brief Set the clusterhead score
   * \param score the candidacy score
   */
  void SetScore (double score);

  /**
   * \brief Get the clusterhead score
   * \return the score
   */
  double GetScore (void) const;

  /**
   * \brief Set the FDMA/TDMA hash value
   * \param hash the hash value
   */
  void SetHash (uint32_t hash);

  /**
   * \brief Get the FDMA/TDMA hash value
   * \return the hash value
   */
  uint32_t GetHash (void) const;

private:
  // ===== Common Fields (Discovery & Election) =====
  MessageType m_messageType;          //!< Message type
  uint32_t m_senderId;                //!< Unique identifier of message sender
  uint8_t m_ttl;                      //!< Time To Live (hops remaining)
  std::vector<uint32_t> m_pathSoFar;  //!< Path So Far (sequence of node IDs)
  Vector m_gpsLocation;               //!< Last Heard GPS location (x, y, z)
  bool m_gpsAvailable;                //!< GPS availability flag

  // ===== Election Announcement Fields =====
  uint16_t m_classId;                 //!< Clusterhead class ID
  uint32_t m_pdsf;                    //!< Predicted Devices So Far
  double m_score;                     //!< Clusterhead candidacy score
  uint32_t m_hash;                    //!< FDMA/TDMA hash function value
};

} // namespace ns3

#endif /* BLE_DISCOVERY_HEADER_H */
