#ifndef STEMREPRESENTATION_H
#define STEMREPRESENTATION_H

#include <vector>
#include <iostream>
#include <ros/ros.h>


#define INFO_STREAM     ROS_INFO_STREAM

//Assumption: stem derivative is always positive

class StemRepresentation
{
    private:

        int m_stem_id;
        int m_n_nodes;
        std::vector<float> m_x_nodes;
        std::vector<float> m_y_nodes;
        std::vector<float> m_z_nodes;
        std::vector<float> m_nearestXYZ;

    public:
        StemRepresentation(int stem_id);
        int getStemID();
        std::vector<float> getStemXYZatZ(float z);
        std::vector<float> getNodesX();
        std::vector<float> getNodesY();
        std::vector<float> getNodesZ();
        bool selfCheck();
        bool isXYZonStem(std::vector<float> xyz);
        void updateNearestXYZ(std::vector<float> from_xyz);
        std::vector<float> getNearestXYZ();
        void loadNodesXYZ(std::vector<float> x, std::vector<float> y, std::vector<float> z);
        int getNumberOfNodes();
        void flipNodes();
        void printAll();

        ~StemRepresentation();
};

#endif // STEMREPRESENTATION_H
