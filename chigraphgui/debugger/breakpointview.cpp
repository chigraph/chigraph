#include "breakpointview.hpp"

#include <chi/NodeInstance.hpp>
#include <chi/GraphModule.hpp>

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


