#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"

class Detector;

/**
 * @brief Klasa abstrakcyjna dla obiektów, które mają otrzymywać informacje
 * o zmianie statusu detektorów.
 */
class Controller
{
public:
    /**
     * @brief Metoda wywoływana gdy detektor ma update statusu.
     * @param detector detektor który ma zaktualizowany status
     */
    virtual void stateChanged(Detector* detector) = 0;
};

#endif // CONTROLLER_H
