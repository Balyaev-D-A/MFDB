#include "comboheader.h"

ComboHeader::ComboHeader(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    margins.left = 0;
    margins.right = 0;
    margins.top = 0;
    margins.bottom = 0;
}

ComboHeader::~ComboHeader()
{
    for (int i=0; i<comboList.count(); i++)
    {
        delete comboList[i];
        comboList[i] = nullptr;
    }
}

void ComboHeader::onSectionCountChanged(int oCount, int nCount)
{

    if (count() > comboList.size())
        while (comboList.size() < nCount)
        {
            comboList.append(new QComboBox(viewport()));
            comboList.last()->hide();
            comboList.last()->addItem("-");
            comboList.last()->addItems(headers);
        }
    else
        while(comboList.size() > nCount)
            comboList.removeLast();
    for (int i=0; i<comboList.size(); i++)
    {
        comboList[i]->setGeometry(sectionViewportPosition(i) + margins.left,
                                 margins.top, sectionSize(i) - margins.right,
                                 height() - margins.bottom);
        comboList[i]->show();
    }
}

void ComboHeader::onSectionResized(int lIndex, int oSize, int nSize)
{

    comboList[lIndex]->setGeometry(sectionViewportPosition(lIndex) + margins.left,
                                  margins.top, nSize - margins.right,
                                  height() - margins.bottom);
}

void ComboHeader::setHeaders(QStringList headers)
{
    this->headers = headers;
}

void ComboHeader::onComboCurrentTextChanged(const QString &text)
{

}
