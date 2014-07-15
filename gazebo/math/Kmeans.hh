/*
 * Copyright (C) 2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef _KMEANS_HH_
#define _KMEANS_HH_

#include <vector>
#include "gazebo/math/Vector3.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace math
  {
    /// \addtogroup gazebo_math
    /// \{

    /// \class Kmeans Kmeans.hh math/gzmath.hh
    /// \brief K-Means clustering algorithm. Given a set of observations,
    /// k-means partitions the observations into k sets so as to minimize the
    /// within-cluster sum of squares.
    /// Description based on http://en.wikipedia.org/wiki/K-means_clustering.
    class GAZEBO_VISIBLE Kmeans
    {
      /// \brief constructor
      /// \param[in] _obs Set of observations to cluster.
      /// \param[in] _k Number of clusters.
      public: Kmeans(const std::vector<Vector3> &_obs, unsigned int _k);

      /// \brief Destructor
      public: virtual ~Kmeans();

      /// \brief Get the observations to cluster.
      /// \return The vector of observations.
      public: std::vector<Vector3> GetObservations();

      /// \brief Set the observations to cluster.
      /// \param[in] _obs The new vector of observations.
      /// \return True if the vector is not empty or false otherwise.
      public: bool SetObservations(const std::vector<Vector3> &_obs);

      /// \brief Get the number of partitions used to cluster.
      /// \return The number of partitions.
      public: unsigned int GetNumClusters();

      /// \brief Set the number of partitions to cluster.
      /// \param[in] _k The number of partitions.
      /// \return true if _k is a valid number (positive).
      public: bool SetNumClusters(unsigned int _k);

      /// \brief Executes the k-means algorithm.
      /// \param[out] _centroids Vector of centroids. Each element contains the
      /// centroid of one cluster.
      /// \param[out] _labels Vector of labels. The size of this vector is
      /// equals to the number of observations. Each element represents the
      /// cluster to which observation belongs.
      /// \return True when the operation succeed or false otherwise. The
      /// operation will fail if the number of observations is not positive,
      /// if the number of clusters is non positive, or if the number of
      /// clusters if greater than the number of observations.
      public: bool Cluster(std::vector<Vector3> &_centroids,
                           std::vector<unsigned int> &_labels);

      /// \brief Given an observation, it returns the closest centroid to it.
      /// \param[in] _p Point to check.
      /// \return The index of the closest centroid to the point _p.
      private: unsigned int ClosestCentroid(const Vector3 &_p);

      /// \brief Check if the given number of observations and number of
      /// clusters is valid. The number of observations has to be positive and
      /// the number of clusters has to be positive and less or equal to the
      /// number of observations.
      /// \return True if the conditions are satisfied and we can run the
      /// algorithm or false otherwise.
      private: bool IsDataValid();

      /// \brief Number of partitions used to cluster.
      private: unsigned int k;

      /// \brief Observations.
      private: std::vector<Vector3> obs;

      /// \brief Centroids.
      private: std::vector<Vector3> centroids;

      /// \brief Centroids from the previous iteration.
      private: std::vector<Vector3> oldCentroids;

      /// \brief Each element represents the cluster to which observation i
      /// belongs.
      private: std::vector<unsigned int> labels;

      /// \brief Used to calculate the centroid of each partition.
      private: std::vector<Vector3> sums;

      /// \brief Used to count the number of observations contained in each
      /// partition.
      private: std::vector<unsigned int> counters;
    };
    /// \}
  }
}

#endif



