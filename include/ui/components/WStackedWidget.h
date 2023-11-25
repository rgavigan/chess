/**
 * @file WStackedWidget.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is an overriden stacked widget class for signalling
 * @date 2023-11-23
 */
#ifndef WSTACKEDWIDGET_H
#define WSTACKEDWIDGET_H

#include <Wt/WStackedWidget.h>
#include <Wt/WSignal.h>
#include <iostream>

class WStackedWidget : public Wt::WStackedWidget {
public:
    // Define a custom signal that will be emitted when setIndex is called
    Wt::Signal<int>& indexChanged() { return indexChanged_; }

    // Override setIndex to include additional logic
    void setCurrentIndex(int index, bool override) {
        // Call the base class implementation
        Wt::WStackedWidget::setCurrentIndex(index);

        // Emit the custom signal
        indexChanged_.emit(index);
    }
private:
    // Custom signal to notify when setIndex is called
    Wt::Signal<int> indexChanged_;
};

#endif // WSTACKEDWIDGET_H
