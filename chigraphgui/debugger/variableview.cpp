#include "variableview.hpp"

#include <lldb/API/SBValue.h>

VariableView::VariableView() {
	setHeaderLabels(QStringList() << i18n("Name") << i18n("Type") << i18n("Summary")
	                              << i18n("Value") << i18n("Obj Desc"));
	
	setDisabled(true);
}

void VariableView::setFrame(lldb::SBFrame frame) {
	clear();

	auto vars = frame.GetVariables(true,  // args
	                               true,  // locals
	                               true,  // statics
	                               true   // in scope only
	                               );

	for (uint32_t idx = 0; idx < vars.GetSize(); ++idx) {
		auto val = vars.GetValueAtIndex(idx);

		auto item = new QTreeWidgetItem(QStringList() << val.GetName() << val.GetDisplayTypeName()
		                                              << val.GetSummary() << val.GetValue()
		                                              << val.GetObjectDescription());

		insertTopLevelItem(0, item);
	}
}
