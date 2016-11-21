#include <QtWidgets/QApplication>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

#include "models.hpp"


static std::unique_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_unique<DataModelRegistry>();
  ret->registerModel(std::make_unique<NaiveDataModel>());

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
