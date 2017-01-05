
#ifndef KCONFIGVIEWSTATESAVER_H
#define KCONFIGVIEWSTATESAVER_H

#include "kviewstateserializer.h"

#include "kconfigwidgets_export.h"

class KConfigGroup;

class KCONFIGWIDGETS_EXPORT KConfigViewStateSaver : public KViewStateSerializer
{
    Q_OBJECT
public:
    explicit KConfigViewStateSaver(QObject *parent = 0);

    /**
      Saves the state to the @p configGroup
    */
    void saveState(KConfigGroup &configGroup);

    /**
      Restores the state from the @p configGroup
    */
    void restoreState(const KConfigGroup &configGroup);
};

#endif
