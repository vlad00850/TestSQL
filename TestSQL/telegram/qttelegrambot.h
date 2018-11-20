#ifndef QTTELEGRAMBOT_H
#define QTTELEGRAMBOT_H

#include <QObject>
#include <QLoggingCategory>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMimeDatabase>
#include <QTimer>

#include "networking.h"
#include "types/chat.h"
#include "types/update.h"
#include "types/user.h"
#include "types/file.h"
#include "types/message.h"
#include "types/reply/genericreply.h"
#include "types/reply/replykeyboardmarkup.h"
#include "types/reply/replykeyboardhide.h"
#include "types/reply/replykeyboardremove.h"
#include "types/reply/forcereply.h"
#include "types/reply/inlinekeyboardmarkup.h"

namespace Telegram {

typedef QList<QList<PhotoSize> > UserProfilePhotos;

class Bot : public QObject
{
    Q_OBJECT
public:
    /**
     * Bot constructor
     * @param Токен
     * @param updates - enable automatic update polling
     * @param updateInterval - interval between update polls in msec
     * @param pollingTimeout - timeout in sec
     * @param parent
     */
    explicit Bot(QString token, bool updates = false, quint32 updateInterval = 1000, quint32 pollingTimeout = 0, QObject *parent = 0);
    ~Bot();

    enum ChatAction { Typing, UploadingPhoto, RecordingVideo, UploadingVideo, RecordingAudio, UploadingAudio, UploadingDocument, FindingLocation };

    /**
     * Returns basic information about the bot in form of a `User` object.
     * Отправляет базовую информацию о боте см. User
     * @return User Object
     * @see https://core.telegram.org/bots/api#getme
     */
    User getMe();

    /**
     * Отправляет текстовые сообщения.
     * @param chatId - Уникальный идетнификатор для сообщения получателю or @channelname
     * @param text - Текст сообщения
     * @param markdown - Use markdown in message display (only Telegram for Android supports this)
     * @param disableWebPagePreview - Disables link previews for links in this message
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendmessage
     */
    bool sendMessage(QVariant chatId, QString text, bool markdown = false, bool disableWebPagePreview = false, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Пересылает сообщение любого типа.
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fromChatId - Unique identifier for the chat where the original message was sent
     * @param messageId - Unique message identifier
     * @return success
     * @see https://core.telegram.org/bots/api#forwardmessage
     */
    bool forwardMessage(QVariant chatId, quint32 fromChatId, quint32 messageId);

    /**
     * Send a photo
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param file - A file to send
     * @param caption - Photo caption
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendphoto
     */
    bool sendPhoto(QVariant chatId, QFile *file, QString caption = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a photo
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param caption - Photo caption
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendphoto
     */
    bool sendPhoto(QVariant chatId, QString fileId, QString caption = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send audio
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param file - A file to send
     * @param duration - Duration of the audio in seconds
     * @param performer - Performer of the audio
     * @param title - Track name of the audio
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendaudio
     */
    bool sendAudio(QVariant chatId, QFile *file, qint64 duration = -1, QString performer = QString(), QString title = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send audio
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent audio
     * @param duration - Duration of the audio in seconds
     * @param performer - Performer of the audio
     * @param title - Track name of the audio
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendaudio
     */
    bool sendAudio(QVariant chatId, QString fileId, qint64 duration = -1, QString performer = QString(), QString title = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a document
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param file - A file to send
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#senddocument
     */
    bool sendDocument(QVariant chatId, QFile *file, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Используйте этот метод для отправки данных. При успешном выполнении возвращается отправленное сообщение.
     * Боты в настоящее время можно отправлять файлы любого типа размером до 50 Мб, это ограничение может быть изменено в будущем.
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#senddocument
     */
    bool sendDocument(QVariant chatId, QString fileId, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Этот объект представляет файл, готовый к загрузке.
     * Он может быть скачан по ссылке вида https://api.telegram.org/file/bot<token>/<file_path>.
     * Ссылка будет действительна как минимум в течение 1 часа.
     * По истечении этого срока она может быть запрошена заново с помощью метода getFile.
     * @param fileId - File identifier to get info about
     * @return File object
     * @see https://core.telegram.org/bots/api#getfile
     */
    File getFile(QString fileId);

    /**
     * Send a sticker
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param file - A file to send
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendsticker
     */
    bool sendSticker(QVariant chatId, QFile *file, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a sticker
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendsticker
     */
    bool sendSticker(QVariant chatId, QString fileId, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a video
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param duration - Duration of sent video in seconds
     * @param caption - Video caption
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendvideo
     */
    bool sendVideo(QVariant chatId, QFile *file, qint64 duration = -1, QString caption = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a video
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param duration - Duration of sent video in seconds
     * @param caption - Video caption
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendvideo
     */
    bool sendVideo(QVariant chatId, QString fileId, qint64 duration = -1, QString caption = QString(), qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a voice
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param duration - Duration of sent audio in seconds
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendvoice
     */
    bool sendVoice(QVariant chatId, QFile *file, qint64 duration = -1, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a voice
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param fileId - Telegram file_id of already sent photo
     * @param duration - Duration of sent audio in seconds
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendvoice
     */
    bool sendVoice(QVariant chatId, QString fileId, qint64 duration = -1, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Send a location
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param latitude - latitude of the location
     * @param longitude - longitude of the location
     * @param replyToMessageId - If the message is a reply, ID of the original message
     * @param replyMarkup - Additional interface options
     * @return success
     * @see https://core.telegram.org/bots/api#sendlocation
     */
    bool sendLocation(QVariant chatId, float latitude, float longitude, qint32 replyToMessageId = -1, const GenericReply &replyMarkup = GenericReply());

    /**
     * Use this method when you need to tell the user that something is happening on the bot's side.
     * @param chatId - Unique identifier for the message recipient or @channelname
     * @param action - Type of action to broadcast
     * @return success
     * @see https://core.telegram.org/bots/api#sendchataction
     */
    bool sendChatAction(QVariant chatId, ChatAction action);

    /**
      * Используйте этот метод для отправки ответов на запросы обратного вызова,
     *  отправленные с встроенных клавиатур. Ответ будет отображаться Пользователю
     *  в виде уведомления в верхней части экрана чата или в виде предупреждения. При успешном выполнении возвращается True.
      * @param callback_query_id - Unique identifier for the query to be answered
      * @param text - Text of the notification. If not specified, nothing will be shown to the user, 0-200 characters
      * @param show_alert - If true, an alert will be shown by the client instead of a notification at the top of the chat screen. Defaults to false.
      * @param url - URL that will be opened by the user's client. If you have created a Game and accepted the conditions via
      *             @Botfather, specify the URL that opens your game – note that this will only work if the query comes from
      *             a callback_game button.
      * @param cache_time - The maximum amount of time in seconds that the result of the callback query may be cached client-side.
      *                     Telegram apps will support caching starting in version 3.14. Defaults to 0.
      */
    bool answerCallbackQuery(QVariant callback_query_id, QString text = QString(), bool show_alert = false, QString url = QString(), quint32 cache_time = 0);

    /**
     * Use this method to get a list of profile pictures for a user.
     * @param userId - Unique identifier of the target user
     * @param offset - Sequential number of the first photo to be returned.
     * @param limit - Limits the number of photos to be retrieved. Values between 1—100 are accepted. Defaults to 100.
     * @return UserProfilePhotos list
     * @see Use this method to get a list of profile pictures for a user.
     */
    UserProfilePhotos getUserProfilePhotos(quint32 userId, qint16 offset =  -1, qint8 limit = -1);

    /**
     * Этот метод используется для получения обновлений через long polling (wiki). Ответ возвращается в виде массива объектов Update.
     * @param timeout - Время в секундах для for long polling.
     * @param limit - Ограничивает число извлекаемых обновлений.
     * @param offset - Идентификатор первого возвращаемого обновления.
     * @return List of Update objects
     * @see https://core.telegram.org/bots/api#getupdates
     */
    QList<Update> getUpdates(quint32 timeout, quint32 limit, quint32 offset);

    /**
     * Этот метод необходим для задания URL вебхука, на который бот будет отправлять обновления.
     * Каждый раз при получении обновления на этот адрес будет отправлен HTTPS POST с сериализованным в JSON объектом Update.
     * При неудачном запросе к вашему серверу попытка будет повторена умеренное число раз.
     * Для большей безопасности рекомендуется включить токен в URL вебхука, например, так: https://yourwebhookserver.com/<token>.
     * Так как никто посторонний не знает вашего токена, вы можете быть уверены, что запросы к вашему вебхуку шлёт именно Telegram
     * @param HTTPS url для отправки запросов. Чтобы удалить вебхук, отправьте пустую строку.
     * @param Загрузка публичного ключа для проверки корневого сертификата. Подробнее в разделе про самоподписанные сертификаты.
     * @return success
     * @see https://core.telegram.org/bots/api#setwebhook
     */
    bool setWebhook(QString url, QFile *certificate);



private:
    Networking *m_net;

    bool _sendPayload(QVariant chatId, QFile *filePayload, ParameterList params, qint32 replyToMessageId, const GenericReply &replyMarkup, QString payloadField, QString endpoint);
    bool _sendPayload(QVariant chatId, QString textPayload, ParameterList params, qint32 replyToMessageId, const GenericReply &replyMarkup, QString payloadField, QString endpoint);

    QJsonObject jsonObjectFromByteArray(QByteArray json);
    QJsonArray jsonArrayFromByteArray(QByteArray json);
    bool responseOk(QByteArray json);

    void internalGetUpdates();
    QTimer *m_internalUpdateTimer;
    quint32 m_updateInterval;
    quint32 m_updateOffset;
    quint32 m_pollingTimeout;


signals:
    void message(Message message);
    void update(Update update);
};

}

#endif // QTTELEGRAMBOT_H
