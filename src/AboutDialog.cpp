#include "AboutDialog.h"
#include "IconFactory.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Acerca de AnSoGicScroll"));
    setMinimumWidth(360);

    auto *layout = new QVBoxLayout(this);

    auto *iconLabel = new QLabel(this);
    iconLabel->setPixmap(IconFactory::appIcon().pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(iconLabel);

    auto *titleLabel = new QLabel(QString("<h2>AnSoGicScroll</h2>"), this);
    titleLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(titleLabel);

    auto *versionLabel = new QLabel(tr("Version %1").arg(QApplication::applicationVersion()), this);
    versionLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(versionLabel);

    auto *descriptionLabel = new QLabel(
        tr("Herramienta ligera de captura de pantalla para la bandeja del "
           "sistema, con soporte para capturas simples y capturas con "
           "desplazamiento (scrolling screenshots).\n\n"
           "Construida con Qt %1.")
            .arg(QT_VERSION_STR),
        this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(descriptionLabel);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttons);
}
