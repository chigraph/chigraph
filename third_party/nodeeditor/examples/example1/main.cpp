#include <QtWidgets/QApplication>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

#include "models.hpp"


static std::unique_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::unique_ptr<DataModelRegistry>(new DataModelRegistry);
  ret->registerModel(std::unique_ptr<NaiveDataModel>(new NaiveDataModel));

  /*
   We could have more models registered.
   All of them become items in the context meny of the scene.

  DataModelRegistry::registerModel<AnotherDataModel>();
  DataModelRegistry::registerModel<OneMoreDataModel>();

  */

  return ret;
}


//------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
