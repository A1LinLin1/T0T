#include <QtWidgets>
#include <QtConcurrent>

#include <functional>

using namespace QtConcurrent;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    const int iterations = 2000;
    QVector<int> vector;
    for (int i = 0; i < iterations; ++i)
        vector.append(i);
    QProgressDialog dialog;
    dialog.setLabelText(QString("Progressing using %1  thread(s)...")
                        .arg(QThread::idealThreadCount()));
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
    QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged, &dialog, &QProgressDialog::setValue);
    std::function<void(int&)> spin = [](int &iteration) {
        const int work = 1000 * 1000 * 40;
        volatile int v = 0;
        for (int j = 0; j < work; ++j)
            ++v;
        qDebug() << "iteration" << iteration << "in thread" << QThread::currentThreadId();
    };
    futureWatcher.setFuture(QtConcurrent::map(vector, spin));
    dialog.exec();
    futureWatcher.waitForFinished();
    qDebug() << "Canceled?" << futureWatcher.future().isCanceled();
}
