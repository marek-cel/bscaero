#ifndef DIALOGRESULTS_H
#define DIALOGRESULTS_H

#include <QDialog>

namespace Ui {
class DialogResults;
}

class DialogResults : public QDialog
{
    Q_OBJECT

public:
    explicit DialogResults(QWidget *parent = nullptr);
    ~DialogResults();

private:
    Ui::DialogResults *ui;
};

#endif // DIALOGRESULTS_H
