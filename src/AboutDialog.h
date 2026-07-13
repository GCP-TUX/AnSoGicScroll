#pragma once

#include <QDialog>

// Simple "About" dialog shown from the tray menu.
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
};
