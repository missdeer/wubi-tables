#include <tuple>

#include <QCoreApplication>
#include <QFile>
#include <QList>
#include <QTextStream>

using Word  = std::tuple<QString, QString>;
using Words = QList<Word>;

bool writeDict(const QString &filePath, const Words &words)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite))
    {
        return false;
    }
    file.seek(0);
    while (!file.atEnd())
    {
        auto line = file.readLine();
        if (line.startsWith("..."))
        {
            break;
        }
    }

    for (const auto &[word, pinyin] : words)
    {
        QString line = word + "\t" + pinyin + "\n";
        file.write(line.toUtf8());
    }

    file.close();
    return true;
}

bool mergeWords(Words &targetWords, const Words &sourceWords)
{
    if (targetWords.isEmpty())
    {
        targetWords = sourceWords;
        return true;
    }

    for (const auto &[word, pinyin] : sourceWords)
    {
        auto iter = std::find_if(targetWords.begin(), targetWords.end(), [&word, &pinyin](const auto &targetWord) {
            return std::get<0>(targetWord) == word && std::get<1>(targetWord) == pinyin;
        });
        if (targetWords.end() != iter)
        {
            // exists, skip
            continue;
        }
        iter = std::find_if(targetWords.begin(), targetWords.end(), [&pinyin](const auto &targetWord) { return std::get<1>(targetWord) == pinyin; });
        if (targetWords.end() != iter)
        {
            auto insertPos = iter;
            while (insertPos != targetWords.end() && std::get<1>(*insertPos) == pinyin)
            {
                ++insertPos;
            }
            if (insertPos == targetWords.end())
            {
                targetWords.append({word, pinyin});
            }
            else
            {
                targetWords.insert(insertPos, {word, pinyin});
            }
        }
        else
        {
            targetWords.append({word, pinyin});
        }
    }
    return true;
}

bool readDict(const QString &filePath, Words &words)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    bool isWord = false;
    while (!file.atEnd())
    {
        auto line = file.readLine().trimmed();
        if (!isWord)
        {
            if (line.startsWith("..."))
            {
                isWord = true;
            }
            continue;
        }
        auto bytes = line.split('\t');
        if (bytes.length() < 2) // 如果有权重，则丢掉权重信息，所以其实不支持权重
        {
            continue;
        }
        if (bytes.at(0).isEmpty() || bytes.at(1).isEmpty())
        {
            continue;
        }
        Word word {QString(bytes.at(0)), QString(bytes.at(1))};
        words.append(word);
    }

    file.close();
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream stream(stdout);
    if (argc < 4)
    {
        stream << "Usage:\n"
               << "    mergetables target.dict.yaml source1.dict.yaml source2.dict.yaml\n";
        return -1;
    }

    Words targetWords;
    if (!readDict(QString::fromLatin1(argv[1]), targetWords))
    {
        stream << "Reading target dict yaml " << QString::fromLatin1(argv[1]) << " failed\n";
        return -1;
    }
    stream << "Read " << targetWords.length() << " words from target dict yaml " << QString::fromLatin1(argv[1]) << "\n";
    stream.flush();

    for (int index = 2; index < argc; index++)
    {
        Words sourceWords;
        if (!readDict(QString::fromLatin1(argv[index]), sourceWords))
        {
            stream << "Reading source dict yaml " << QString::fromLatin1(argv[index]) << " failed\n";
            continue;
        }
        stream << "Read " << sourceWords.length() << " words from source dict yaml " << QString::fromLatin1(argv[index]) << "\n";
        stream.flush();
        if (!mergeWords(targetWords, sourceWords))
        {
            stream << "Merging source dict words from " << QString::fromLatin1(argv[index]) << " failed\n";
            continue;
        }
        stream << "Merged " << sourceWords.length() << " words from source dict yaml " << QString::fromLatin1(argv[index]) << " to target.\n";
        stream.flush();
    }

    if (!writeDict(QString::fromLatin1(argv[1]), targetWords))
    {
        stream << "Writing target dict yaml " << QString::fromLatin1(argv[1]) << " failed\n";
        return -1;
    }

    stream << "Merged ";
    for (int index = 2; index < argc; index++)
    {
        stream << QString::fromLatin1(argv[index]) << " ";
    }
    stream << "to " << QString::fromLatin1(argv[1]) << ", total word count:" << targetWords.length() << "\n";
    stream.flush();

    return 0;
}
