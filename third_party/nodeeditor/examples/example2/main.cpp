#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include <nodes/DataModelRegistry>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"


static std::unique_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::unique_ptr<DataModelRegistry>(new DataModelRegistry);
  ret->registerModel(std::unique_ptr<TextSourceDataModel>(new TextSourceDataModel));

  ret->registerModel(std::unique_ptr<TextSourceDataModel>(new TextSourceDataModel));

  return ret;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  FlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
