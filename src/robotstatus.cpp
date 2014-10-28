#include "robotstatus.h"

RobotStatus::RobotStatus(int n_joints_to_monitor, RobotConfig* robot_config)
{
    if(n_joints_to_monitor <= 0){
        INFO_STREAM("trying to initialize robot status object with zero or negative number of joints to monitor!");
    }
    m_joints_to_monitor = KDL::JntArray(n_joints_to_monitor);
    m_n_joints_monitoring = n_joints_to_monitor;

    // robot status currently configured for joint names:
    m_joint_names.push_back("torso_joint");
    if(robot_config->isLeftArmPreferred()){
        m_joint_names.push_back("shoulder_yaw_joint_left");
        m_joint_names.push_back("shoulder_pitch_joint_left");
        m_joint_names.push_back("shoulder_roll_joint_left");
        m_joint_names.push_back("elbow_pitch_joint_left");
        m_joint_names.push_back("elbow_roll_joint_left");
        m_joint_names.push_back("wrist_pitch_joint_left");
        m_joint_names.push_back("wrist_yaw_joint_left");
    } else if (robot_config->isRightArmPreferred()){
        m_joint_names.push_back("shoulder_yaw_joint_right");
        m_joint_names.push_back("shoulder_pitch_joint_right");
        m_joint_names.push_back("shoulder_roll_joint_right");
        m_joint_names.push_back("elbow_pitch_joint_right");
        m_joint_names.push_back("elbow_roll_joint_right");
        m_joint_names.push_back("wrist_pitch_joint_right");
        m_joint_names.push_back("wrist_yaw_joint_right");
    } else {
        INFO_STREAM("trying to initialize robot status without knowing whether we use left or right arm");
    }


}

std::vector<std::string> RobotStatus::getJointNames(){
    return m_joint_names;
}

bool RobotStatus::selfCheck(){

    bool IamOK = true;

    if(m_n_joints_monitoring <= 0){
        INFO_STREAM("trying to use robot status object while number of joints to monitor is zero or negative!");
        IamOK = false;
    }

    return IamOK;
}

void RobotStatus::receivedTorsoMsg(const sensor_msgs::JointState & msg){

    if(!selfCheck()){
        return;
    }

    m_last_update = ros::Time::now();

    m_joints_to_monitor(0) = msg.position[0];
}

void RobotStatus::receivedArmMsg(const sensor_msgs::JointState & msg){

    if(!selfCheck()){
        return;
    }

    for(int i = 1; i < 8; ++i){
        m_joints_to_monitor(i) = msg.position[i-1];
    }
    m_last_update = ros::Time::now();

}

ros::Time RobotStatus::getLastUpdateTime(){
    return m_last_update;
}

double RobotStatus::getTimeSinceLastUpdate(){
    // returns n seconds since last update in any of
    // the joints we are monitoring
    ros::Duration interval = ros::Time::now() - RobotStatus::getLastUpdateTime();
    return interval.toSec();
}

bool RobotStatus::isUpToDate(){
    if(getTimeSinceLastUpdate() < m_up_to_date_threshold){
        return true;
    } else {
        return false;
    }
}

KDL::JntArray RobotStatus::getJointStatus(){
    return m_joints_to_monitor;
}

void RobotStatus::setUpToDateThreshold(double threshold){
    m_up_to_date_threshold = threshold;
}

std::vector<float> RobotStatus::getGripperXYZ(RobotConfig* robot_config ){

    KDL::Frame cartpos = getGripperKDLframe(robot_config);

    m_gripper_xyz.clear();
    m_gripper_xyz.push_back(cartpos.p.x());
    m_gripper_xyz.push_back(cartpos.p.y());
    m_gripper_xyz.push_back(cartpos.p.z());

    return m_gripper_xyz;

}

KDL::Frame RobotStatus::getGripperKDLframe(RobotConfig* robot_config ){

    KDL::ChainFkSolverPos_recursive forward_kinematics_solver = KDL::ChainFkSolverPos_recursive(robot_config->getKinematicChain());

    int fk_ret = forward_kinematics_solver.JntToCart(getJointStatus(),m_gripper_kdlframe);

    if( fk_ret < 0 ){
        INFO_STREAM("Warning: something wrong in solving forward kinematics in getGripperKDLframe");
    }

    return m_gripper_kdlframe;

}

bool RobotStatus::isGripperXYZvalid(){
    if(m_gripper_xyz.size() == 3){
        return true;
    } else {
        return false;
    }
}

void RobotStatus::printAll(){

    INFO_STREAM("===============");
    INFO_STREAM("Robot status: ");
    //    INFO_STREAM("\t initialized with robot config of: " << m_robot_config.getName());
    for(int i; i<m_n_joints_monitoring; ++i){
        INFO_STREAM("\t" << m_joints_to_monitor(i));
    }
    INFO_STREAM("===============");
}

RobotStatus::~RobotStatus(){
    // destructor
}
