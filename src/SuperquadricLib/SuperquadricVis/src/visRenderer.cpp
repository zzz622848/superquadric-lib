/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @authors: Giulia Vezzani <giulia.vezzani@iit.it>
 */

#include "visRenderer.h"

using namespace std;
using namespace SuperqVis;
using namespace SuperqModel;
using namespace SuperqGrasp;
using namespace Eigen;

/**********************************************/
Visualizer::Visualizer()
{
    height = -0.2;
    size_points = 4;
    backgroundColor = {1.0,1.0,1.0};

    closing = false;

    vtk_renderer = vtkSmartPointer<vtkRenderer>::New();
    vtk_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

    vtk_renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtk_renderWindowInteractor->SetRenderWindow(vtk_renderWindow);

    vtk_renderWindow->SetSize(600,600);
    vtk_renderWindow->AddRenderer(vtk_renderer);

    vtk_renderer->SetBackground(backgroundColor.data());

    vtk_axes = vtkSmartPointer<vtkAxesActor>::New();
    vtk_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    vtk_widget->SetOutlineColor(0.9300,0.5700,0.1300);
    vtk_widget->SetOrientationMarker(vtk_axes);
    vtk_widget->SetInteractor(vtk_renderWindowInteractor);
    vtk_widget->SetViewport(0.0,0.0,0.2,0.2);
    vtk_widget->SetEnabled(1);
    vtk_widget->InteractiveOn();

    vtk_style = vtkSmartPointer<vtkInteractorStyleSwitch>::New();
    vtk_style->SetCurrentStyleToTrackballCamera();
    vtk_renderWindowInteractor->SetInteractorStyle(vtk_style);
}

/**********************************************/
Visualizer::~Visualizer()
{
    closing=true;
}

/**********************************************/
void Visualizer::addPoints(PointCloud &point_cloud, const bool &show_downsample)
{
    vector<Vector3d> &all_points = point_cloud.points_for_vis;
    vector<vector<unsigned char>> all_colors = point_cloud.colors;

    size_points = 4;
    vtk_all_points = unique_ptr<PointsVis>(new PointsVis(all_points,size_points));
    vtk_all_points->set_colors(all_colors);

    vtk_renderer->AddActor(vtk_all_points->get_actor());

    if (show_downsample)
    {
        size_points = 8;
        vector<Vector3d> &dwn_points = point_cloud.points;
        vtk_dwn_points = unique_ptr<PointsVis>(new PointsVis(dwn_points,size_points));
        vtk_dwn_points->get_actor()->GetProperty()->SetColor(1.0,0.0,0.0);
        vtk_renderer->AddActor(vtk_dwn_points->get_actor());
    }

    vector<double> bounds(6),centroid(3);
    vtk_all_points->get_polydata()->GetBounds(bounds.data());

    for (size_t i = 0; i < centroid.size(); i++)
        centroid[i] = 0.5*(bounds[i << 1] + bounds[(i << 1) + 1]);

    vtk_camera = vtkSmartPointer<vtkCamera>::New();
    vtk_camera->SetPosition(centroid[0] + 0.5,centroid[1],centroid[2] + 0.4);
    vtk_camera->SetFocalPoint(centroid.data());
    vtk_camera->SetViewUp(0.0,0.0,1.0);
    vtk_renderer->SetActiveCamera(vtk_camera);
}

/**********************************************/
void Visualizer::addPlane(const double &z)
{
    vtk_plane = unique_ptr<PlaneVis>(new PlaneVis(-z));
    vtk_renderer->AddActor(vtk_plane->get_actor());
}

/**********************************************/
void Visualizer::addSuperq(vector<SuperqModel::Superquadric> &s)
{
    Vector11d r;

    for (auto sup:s)
    {   r.resize(12);
        r.segment(0,3) = sup.getSuperqCenter();
        r.segment(3,4) = sup.getSuperqAxisAngle();
        r.segment(7,3) = sup.getSuperqDims();
        r.segment(10, 2) = sup.getSuperqExps();

        vtk_superquadric = unique_ptr<SuperquadricVis>(new SuperquadricVis(r));

        vtk_renderer->AddActor(vtk_superquadric->get_actor());
    }

    vtk_camera = vtkSmartPointer<vtkCamera>::New();

    Vector3d center(3);
    if (s.size() == 1)
      center = s[0].getSuperqCenter();
    else
    {
        center.setZero();
        for (auto sup:s)
        {
          center += sup.getSuperqCenter();
        }

        center /=s .size();
    }

    vtk_camera->SetPosition(center(0)+0.5,center(1),center(2)+0.4);
    vtk_camera->SetFocalPoint(center.data());
    vtk_camera->SetViewUp(0.0,0.0,1.0);
    vtk_renderer->SetActiveCamera(vtk_camera);
}

/**********************************************/
void Visualizer::addPoses(const vector<SuperqGrasp::GraspPoses> &poses)
{
    Vector6d pose_vect;
    double offset=0.0;
    int i=0;

    for (auto pose : poses)
    {
        if (pose.getGraspParams().norm() > 0.0)
        {
            pose_vect = pose.getGraspParams();
            vtkSmartPointer<vtkAxesActor> ax_actor = vtkSmartPointer<vtkAxesActor>::New();
            vtkSmartPointer<vtkCaptionActor2D> cap_actor = vtkSmartPointer<vtkCaptionActor2D>::New();
            ax_actor->VisibilityOff();
            cap_actor->VisibilityOff();
            pose_actors.push_back(ax_actor);
            pose_captions.push_back(cap_actor);
            vtk_renderer->AddActor(ax_actor);

            vtk_renderer->AddActor(cap_actor);

            shared_ptr<PoseVis> candidate_pose = shared_ptr<PoseVis>(new PoseVis);

            candidate_pose->setvtkTransform(pose_vect);
            candidate_pose->pose_vtk_actor->SetUserTransform(candidate_pose->pose_vtk_transform);
            ax_actor->SetUserTransform(candidate_pose->pose_vtk_transform);

            candidate_pose->pose_vtk_actor->ShallowCopy(ax_actor);
            ax_actor->AxisLabelsOff();
            ax_actor->SetTotalLength(0.02, 0.02, 0.02);
            ax_actor->SetShaftTypeToCylinder();
            ax_actor->VisibilityOn();

            cap_actor->VisibilityOn();
            cap_actor->GetTextActor()->SetTextScaleModeToNone();

            stringstream ss;
            ss << pose.getHandName()<<" : "<<setprecision(3)<<pose.cost;

            offset += 0.01;

            candidate_pose->setvtkActorCaption(ss.str(), offset);
            cap_actor->SetCaption(candidate_pose->pose_vtk_caption_actor->GetCaption());
            cap_actor->BorderOff();
            cap_actor->LeaderOn();
            cap_actor->GetCaptionTextProperty()->SetFontSize(15);
            cap_actor->GetCaptionTextProperty()->FrameOff();
            cap_actor->GetCaptionTextProperty()->ShadowOff();

            cap_actor->GetCaptionTextProperty()->BoldOff();
            cap_actor->GetCaptionTextProperty()->ItalicOff();
            cap_actor->GetCaptionTextProperty()->SetColor(0.1, 0.1, 0.1);

            // This should be done only for best pose (TO UPDATE)
            cap_actor->GetCaptionTextProperty()->BoldOn();
            cap_actor->GetCaptionTextProperty()->SetColor(0., 0.35, 0.0);
            cap_actor->GetCaptionTextProperty()->SetFontSize(20);

            cap_actor->SetAttachmentPoint(candidate_pose->pose_vtk_caption_actor->GetAttachmentPoint());

            i++;
            pose_candidates.push_back(candidate_pose);
        }
    }

}
/**********************************************/
void Visualizer::visualize()
{
    vtk_renderWindowInteractor->Initialize();
    vtk_renderWindowInteractor->CreateRepeatingTimer(10);

    vtk_updateCallback = vtkSmartPointer<UpdateCommand>::New();

    vtk_updateCallback->set_closing(closing);
    vtk_renderWindowInteractor->AddObserver(vtkCommand::TimerEvent,vtk_updateCallback);

    vtk_renderWindowInteractor->Start();
}

/**********************************************/
void Visualizer::saveScreenshot(const string &object, const int &number)
{
    vtk_renderWindowInteractor->Render();

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
    vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(vtk_renderWindow);
    windowToImageFilter->SetScale(1);
    windowToImageFilter->ReadFrontBufferOff();
    windowToImageFilter->Update();

    vtkSmartPointer<vtkPNGWriter> writer =
    vtkSmartPointer<vtkPNGWriter>::New();

    stringstream ss;
    ss << number;
    string number_str = ss.str();

    writer->SetFileName((object + "_screenshot_no_" + number_str + ".png").c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

    vtk_renderWindowInteractor->GetRenderWindow()->Finalize();
    vtk_renderWindowInteractor->TerminateApp();
}
