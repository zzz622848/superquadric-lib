/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @authors: Giulia Vezzani <giulia.vezzani@iit.it>
 */

#include "poseVis.h"

using namespace std;
using namespace Eigen;
using namespace SuperqVis;


/**********************************************/
PoseVis::PoseVis()
{
    pose.setIdentity();
    pose_vtk_actor = vtkSmartPointer<vtkAxesActor>::New();
    pose_vtk_transform = vtkSmartPointer<vtkTransform>::New();
    pose_vtk_caption_actor = vtkSmartPointer<vtkCaptionActor2D>::New();
}

/**********************************************/
void PoseVis::setvtkTransform(const VectorXd &pose_vect)
{
    if (pose_vect.size() == 6 and pose_vect.norm() > 0.0)
    {
        pose.block<3,3>(0,0) = (AngleAxisd(pose_vect(3), Vector3d::UnitZ())*
                                AngleAxisd(pose_vect(4), Vector3d::UnitY())*
                                AngleAxisd(pose_vect(5), Vector3d::UnitZ())).toRotationMatrix();

        pose.block<3,1>(0,3) = pose_vect.head(3);

        vtkSmartPointer<vtkMatrix4x4> m_vtk = vtkSmartPointer<vtkMatrix4x4>::New();
        m_vtk->Zero();
        for (auto i = 0; i < pose.rows(); i++)
        {
            for(auto j = 0; j < pose.cols(); j++)
            {
                m_vtk->SetElement(i, j, pose(i, j));
            }
        }

        pose_vtk_transform->SetMatrix(m_vtk);
    }
}

/**********************************************/
void PoseVis::setvtkActorCaption(const string &caption, double &offset)
{
    pose_vtk_caption_actor->GetTextActor()->SetTextScaleModeToNone();
    pose_vtk_caption_actor->SetCaption(caption.c_str());
    pose_vtk_caption_actor->BorderOff();
    pose_vtk_caption_actor->LeaderOn();
    pose_vtk_caption_actor->GetCaptionTextProperty()->SetFontSize(20);
    pose_vtk_caption_actor->GetCaptionTextProperty()->FrameOff();
    pose_vtk_caption_actor->GetCaptionTextProperty()->ShadowOff();
    pose_vtk_caption_actor->GetCaptionTextProperty()->BoldOff();
    pose_vtk_caption_actor->GetCaptionTextProperty()->ItalicOff();
    pose_vtk_caption_actor->SetAttachmentPoint(pose(0,3), pose(1,3), pose(2,3)+offset);
}
