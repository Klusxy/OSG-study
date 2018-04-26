#include <string>
#include <windows.h>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/AnimationPath>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

std::string osgModelPath("..//..//..//OSGPractise//Resource//");

osg::AnimationPath* createAnimationPath(const osg::Vec3 &center, float radius, double loopTime)
{
	osg::AnimationPath *ap = new osg::AnimationPath;
	ap->setLoopMode(osg::AnimationPath::LOOP);
	//设置关键点数
	int numSamples = 40;
	float yaw = 0.0f;
	float yaw_delta = 2.0f * osg::PI / ((float)numSamples - 1.0f);//将360度平分(弧度)
	float roll = osg::inDegrees(30.0f);
	//设置时间间隔
	double time = 0.0f;
	double time_delta = loopTime / numSamples;
	for (int i =0; i < numSamples; ++i)
	{
		osg::Vec3 position(0, 0, 0);
		osg::Quat rotation(osg::Quat(roll, osg::Vec3(0.0, 1.0, 0.0)) * osg::Quat(-yaw, osg::Vec3(0.0, 0.0, 1.0)));
		ap->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
		yaw += yaw_delta;
		time += time_delta;
	}
/*
	ap->insert(time, osg::AnimationPath::ControlPoint(osg::Vec3(0.0f, 0.0f, 0.0f)));
	ap->insert(1.0f, osg::AnimationPath::ControlPoint(osg::Vec3(100.0f, 0.0f, 0.0f)));*/
	return ap;
}

osg::Node* createMovingModel(osg::Vec3 &center, float &raidus)
{
	float animationLength = 10.0f;
	osg::AnimationPath *ap = createAnimationPath(center, raidus, animationLength);
	osg::Group *model = new osg::Group;
	osg::ref_ptr<osg::Node> fountain = osgDB::readNodeFile(osgModelPath + "fountain.osgt");
	if (fountain)
	{
		fountain->asGroup()->getChild(0)->setNodeMask(0);
		osg::MatrixTransform *xform = new osg::MatrixTransform;
		//设置更新回调
		xform->setUpdateCallback(new osg::AnimationPathCallback(ap));
		xform->addChild(fountain);
		model->addChild(xform);
	}
	return model;
}

osg::Node* createModel()
{
	osg::Group *root = new osg::Group;
	float radius = 1.0f;
	osg::Vec3 center(0.0f, 0.0f, 0.0f);
	root->addChild(createMovingModel(center, radius));
	//root->addChild(osgDB::readNodeFile(osgModelPath + "cow.osgt"));
	return root;
}

void setupWindow(osgViewer::Viewer &viewer)
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 1920 + 60;
	traits->y = 60;
	traits->width = 800;
	traits->height = 600;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	/*
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);*/
	viewer.addSlave(camera);
}

int main(int argc, char** argv)
{
	osgViewer::Viewer viewer;
	setupWindow(viewer);
	osg::Node *root = createModel();
	viewer.setSceneData(root);
	viewer.realize();
	viewer.run();
	return 1;
}