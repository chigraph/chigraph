#include "breakpointview.hpp"

#include <chi/NodeInstance.hpp>
#include <chi/GraphModule.hpp>

#include "../mainwindow.hpp"
#include "../functiontabview.hpp"

class BreakpointView::BreakpointItem : public QTreeWidgetItem {
public:
	BreakpointItem(chi::NodeInstance& inst) : mInst{&inst} {
		setText(0, QString::fromStdString(inst.function().qualifiedName()));
		setText(1, QString::fromStdString(inst.stringId()));
	}
	
	chi::NodeInstance& instance() const { return *mInst; }
private:
	chi::NodeInstance* mInst;
};


BreakpointView::BreakpointView() {
	setHeaderLabels(QStringList() << i18n("Function") << i18n("ID"));
	
	connect(this, &QTreeWidget::itemDoubleClicked, this, [](QTreeWidgetItem* item) {
		auto casted = dynamic_cast<BreakpointItem*>(item);
		
		if (casted != nullptr) {
			MainWindow::instance()->tabView().centerOnNode(casted->instance());
			MainWindow::instance()->tabView().selectNode(casted->instance());
		}
	});
}

void BreakpointView::addBreakpoint(chi::NodeInstance& inst)
{
	auto item = new BreakpointItem(inst);
	mBreakpoints.emplace(&inst, item);
	addTopLevelItem(item);
}

bool BreakpointView::removeBreakpoint(chi::NodeInstance& inst)
{
	auto iter = mBreakpoints.find(&inst);
	if (iter == mBreakpoints.end()) {
		return false;
	}
	
	removeItemWidget(iter->second, 0);
	mBreakpoints.erase(iter);
	
	return true;
}


