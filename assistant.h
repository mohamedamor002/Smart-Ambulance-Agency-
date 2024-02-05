
#ifndef ASSISTANT_H
#define ASSISTANT_H
#include <QString>
#include <QPlainTextEdit>




class assistant
{
public:
    assistant();
   static void generate_and_display_text(const QString& prompt, const QString& model, const QString& api_key, QPlainTextEdit* label);

};

#endif // ASSISTANT_H
