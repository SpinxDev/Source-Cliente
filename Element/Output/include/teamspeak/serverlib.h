/*
 * This is the main header for the TeamSpeak 3 Server SDK. All the functions that are used
 * to communicate with the TeamSpeak 3 Server are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3server_freeMemory()
 * after finishing with the result. This holds true for all the functions returning
 * strings (so the parameter is "char** result"). Again, the details are all in the
 * documentation.
 */

#ifndef SERVERLIB_H
#define SERVERLIB_H

#include <teamspeak/public_definitions.h>
#include <teamspeak/server_commands_file_transfer.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TS3ChannelCreationParams;        //opaque definition
struct TS3VirtualServerCreationParams;  //opaque definition
struct TS3Variables;                    //opaque definition

enum VirtualServerCreateFlags {
	VIRTUALSERVER_CREATE_FLAG_NONE                = 0x0000, ///< Server password is stored plaintext and will be encrypted by the server library before stored.
	VIRTUALSERVER_CREATE_FLAG_PASSWORDS_ENCRYPTED = 0x0001, ///< Server password is already encrypted in the creation parameters. Will be stored as is.
};

enum ChannelCreateFlags {
	CHANNEL_CREATE_FLAG_NONE                = 0x000, ///< Channel password is stored plaintext and will be encrypted by the server library before stored.
	CHANNEL_CREATE_FLAG_PASSWORDS_ENCRYPTED = 0x001, ///< Channel passwords are already encrypted in the creation parameters. Will be stored as is.
};

/**
 * Server callbacks.
 * Zero initialize and set members to functions that are to be called when the event in question happens.
 * Every callback you use should exit quickly to avoid stalling the server. If you need any expensive activity
 * upon receiving callbacks, consider starting the activity in a new thread and allow the callback to exit quickly.
*/
struct ServerLibFunctions {
	/**
	 * @brief called when audio data is received from any client. Allows access to audio data from any client.
	 *
	 * @details Can be used to implement server side voice recording. Do not implement if you don't need server side recording.
	 * Callback will be called for every client sending audio data, even if nobody can hear said client (e.g. alone in a channel).
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client which is sending audio data
	 * @param voiceData pointer to the voice buffer. Must not be invalidated or otherwise tampered with.
	 * @param voiceDataSize number of audio frames available in the buffer
	 * @param frequency audio data sample rate
	*/
	void (*onVoiceDataEvent)           (uint64 serverID, anyID clientID, unsigned char* voiceData, unsigned int voiceDataSize, unsigned int frequency);

	/**
	 * @brief called when a client starts talking
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client that started talking
	*/
	void (*onClientStartTalkingEvent)  (uint64 serverID, anyID clientID);

	/**
	 * @brief called when a client stops talking
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client that stopped talking
	*/
	void (*onClientStopTalkingEvent)   (uint64 serverID, anyID clientID);

	/**
	 * @brief called when a client connects
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client that connected
	 * @param channelID the channel that the client connected to
	 * @param removeClientError whether to allow the client on the server. Set the value to one of the values from the @ref Ts3ErrorType enum if you want to reject the client.
	*/
	void (*onClientConnected)          (uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError);

	/**
	 * @brief called when a client disconnects
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client that disconnected. The client is already gone by the time this callback is called. The client id cannot be used to query information.
	 * @param channelID the channel that the client was in before disconnecting.
	*/
	void (*onClientDisconnected)       (uint64 serverID, anyID clientID, uint64 channelID);

	/**
	 * @brief called when a client changed to a different channel by any means, including switching the channel themselves.
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client that switched to a different channel.
	 * @param oldChannelID the previous channel the client was in.
	 * @param newChannelID the current channel the client is in now.
	*/
	void (*onClientMoved)              (uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID);

	/**
	 * @brief called when a channel has been created
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID the id of the client that created the channel. 0 if the server created the channel.
	 * @param channelID the id of the newly created channel.
	*/
	void (*onChannelCreated)           (uint64 serverID, anyID invokerClientID, uint64 channelID);

	/**
	 * @brief called when a channel has been edited
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID the client that edited the channel. 0 if the server edited the channel.
	 * @param channelID the channel that was edited
	*/
	void (*onChannelEdited)            (uint64 serverID, anyID invokerClientID, uint64 channelID);

	/**
	 * @brief called when a channel was deleted
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID client that deleted the channel. 0 if the server deleted the channel
	 * @param channelID the id of the channel that was deleted. The channel is gone already by the time this callback is called and information about the channel is no longer available
	*/
	void (*onChannelDeleted)           (uint64 serverID, anyID invokerClientID, uint64 channelID);

	/**
	 * @brief called when a server wide text message was sent
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID the client that is sending the message
	 * @param textMessage utf8 encoded c string containing the text of the message sent
	*/
	void (*onServerTextMessageEvent)   (uint64 serverID, anyID invokerClientID, const char* textMessage);

	/**
	 * @brief called when a channel text message was sent
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID the client that is sending the message
	 * @param targetChannelID the channel in which the message is sent
	 * @param textMessage utf8 encoded c string containing the message sent
	*/
	void (*onChannelTextMessageEvent)  (uint64 serverID, anyID invokerClientID, uint64 targetChannelID, const char* textMessage);

	/**
	 * @brief when user logging was enabled when calling @ref ts3server_initServerLib this callback is called whenever a message with at least the severity
	 * specified through @ref ts3server_setLogVerbosity is supposed to be logged. Allows to customize logging and handle errors or critical log events.
	 *
	 * @param logmessage utf8 encoded c string containing the message to be logged
	 * @param logLevel the severity of the message that the callback is called for. One of the values from the @ref LogLevel enum
	 * @param logChannel utf8 encoded c string containing the arbitrary text used for grouping messages.
	 * @param logID the server on which the message was logged
	 * @param logTime utf8 encoded c string containing the time and date in system format the message was logged
	 * @param completeLogString utf8 encoded c string containing all the previous parameters in a complete text string ready for logging.
	*/
	void (*onUserLoggingMessageEvent)  (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);

	/**
	 * @brief called when an error occurs with license checking.
	 *
	 * @details Allows you to gracefully handle errors like a missing or expired license for example, while keeping the rest of your application running.
	 *
	 * @param serverID the server on which the error occured. This server has been shut down automatically, other servers keep running.
	 * If this is 0 then all servers are affected by the error and have been shut down. In this case you may want to call @ref ts3server_destroyServerLib to clean up resources.
	 * @param errorCode the error that appeared. One of the values from the @ref Ts3ErrorType enum. You can use @ref ts3server_getGlobalErrorMessage to get a string representation for the error code.
	*/
	void (*onAccountingErrorEvent)     (uint64 serverID, unsigned int errorCode);

	/**
	 * @brief called when a packet needs to be encrypted to be sent over the wire.
	 *
	 * @details Used to implement custom encryption of server communication. This needs to be implemented the same in the client and server, otherwise clients cannot communicate with the server.
	 * Only implement this callback when you need custom encryption.
	 *
	 * @param dataToSend pointer to an array of bytes that need to be encrypted. Must not be freed. Encrypt the data in place in this array if the size of your encrypted data
	 * is smaller than indicated in the sizeOfData parameter. Otherwise allocate your own memory and replace the pointer to point to your own allocated memory. In this case you need to take care of freeing the memory.
	 * @param sizeOfData size in byte of the dataToSend array.
	*/
	void (*onCustomPacketEncryptEvent) (char** dataToSend, unsigned int* sizeOfData);

	/**
	 * @brief called when a packet needs to be decrypted after it has been received.
	 *
	 * @details Used to implement custom encryption of server communication. This needs to be implemented the same in the client and server, otherwise clients cannot communicate with the server.
	 * Only implement this callback when you need custom encryption.
	 *
	 * @param dataReceived pointer to an array of bytes that need to be decrypted. Must not be freed. Decrypt the data in place in this array if the size of your decrypted data
	 * is smaller than indicated by the dataReceivedSize parameter. Otherwise allocate your own memory and replace the pointer to point to your own allocated memory. In this case you need to take care of freeing the memory
	 * @param dataReceivedSize size in byte of the dataReceived array.
	*/
	void (*onCustomPacketDecryptEvent) (char** dataReceived, unsigned int* dataReceivedSize);

	/**
	 * @brief called whenever a file transfer is done
	 *
	 * @param data pointer to a structure describing the file transfer that completed. See @ref FileTransferCallbackExport for details.
	*/
	void (*onFileTransferEvent)        (const struct FileTransferCallbackExport* data);

	/**
	 * @brief called when a client is about to connect. Can be used to deny clients from connecting.
	 *
	 * Return @ref ERROR_ok to allow the client on the server, or @ref ERROR_permissions to reject the client.
	 *
	 * @param serverID the server the client wants to connect to
	 * @param client pointer to a @ref ClientMiniExport describing the client trying to connect
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientCanConnect)               (uint64 serverID, const struct ClientMiniExport* client);

	/**
	 * @brief called when a client requests channel description of a channel using @ref ts3client_requestChannelDescription. Can be used to deny access to channel descriptions.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server on which the request was received
	 * @param client pointer to a @ref ClientMiniExport describing the client requesting the channel description
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientCanGetChannelDescription) (uint64 serverID, const struct ClientMiniExport* client);

	/**
	 * @brief called when a client wants to update a clients variables. Used to deny or allow updating certain client variables
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param clientID the client for which the variables are attempted to be changed.
	 * @param variables pointer to a @ref VariablesExport containing the variables, new and old values of the client.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientUpdate)                   (uint64 serverID, anyID clientID, const struct VariablesExport* variables);

	/**
	 * @brief called before a client is kicked from the channel. Allows you to control whether clients are allowed to kick another client
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client pointer to a @ref ClientMiniExport describing the client attempting to kick another client.
	 * @param toKickCount number of clients that are supposed to be kicked
	 * @param toKickClients array of @ref ClientMiniExport describing the clients to be kicked
	 * @param reasonText utf8 encoded c string containing the reason for the kick provided.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientKickFromChannel)          (uint64 serverID, const struct ClientMiniExport* client, int toKickCount, const struct ClientMiniExport* toKickClients, const char* reasonText);

	/**
	 * @brief called before a client is kicked from the server. Allows you to control whether clients are allowed to kick another client
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client pointer to a @ref ClientMiniExport describing the client attempting to kick another client.
	 * @param toKickCount number of clients that are supposed to be kicked
	 * @param toKickClients array of @ref ClientMiniExport describing the clients to be kicked
	 * @param reasonText utf8 encoded c string containing the provided reason for the kick.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientKickFromServer)           (uint64 serverID, const struct ClientMiniExport* client, int toKickCount, const struct ClientMiniExport* toKickClients, const char* reasonText);

	/**
	 * @brief called when a client requests to move one or more other clients. Allows you to control whether a client can move the clients.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server on which the move is attempted.
	 * @param client pointer to a @ref ClientMiniExport describing the client attempting to move the client(s).
	 * @param toMoveCount number of clients that are being moved.
	 * @param toMoveClients array of @ref ClientMiniExport describing which clients are being moved.
	 * @param newChannel id of the channel the clients are to be moved in to.
	 * @param reasonText utf8 encoded c string containing the reason provided for the move.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permClientMove)                     (uint64 serverID, const struct ClientMiniExport* client, int toMoveCount, const struct ClientMiniExport* toMoveClients, uint64 newChannel, const char* reasonText);

	/**
	 * @brief called when a client attempts to move a channel. Allows you to control whether the client is allowed to move the channel.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client a @ref ClientMiniExport describing the client attempting to move the channel.
	 * @param channelID the channel to be moved.
	 * @param newParentChannelID the new parent channel of the channel
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permChannelMove)                    (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, uint64 newParentChannelID);

	/**
	 * @brief called when a client tries to send a message. Allows you to control whether the client is allowed to send the message.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client a @ref ClientMiniExport describing the client attempting to send the message
	 * @param targetMode describing the type of message attempting to be sent. One of the values from the @ref TextMessageTargetMode enum
	 * @param targetClientOrChannel id of the channel or client (depending of the targetMode) that the message is sent to.
	 * @param textMessage utf8 encoded c string containing the message to be sent.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permSendTextMessage)                (uint64 serverID, const struct ClientMiniExport* client, anyID targetMode, uint64 targetClientOrChannel, const char* textMessage);

	/**
	 * @brief called when server connection information is requested using @ref ts3client_requestServerConnectionInfo. Can be used to deny access.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client a @ref ClientMiniExport describing the client requesting the action
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permServerRequestConnectionInfo)    (uint64 serverID, const struct ClientMiniExport* client);

	/**
	 * @brief called when a client attempts to request another clients connection variables using @ref ts3client_requestConnectionInfo
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client requesting the other clients information
	 * @param mayViewIpPort pointer to a variable that controls whether the IP and port of the target client may be seen by the client. Set to 1 to allow the requesting client to see the IP and port. Set to 0 to deny IP and port.
	 * @param targetClient describes the client that the connection information is requested for.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permSendConnectionInfo)             (uint64 serverID, const struct ClientMiniExport* client, int* mayViewIpPort, const struct ClientMiniExport* targetClient);

	/**
	 * @brief called when a client attempts to create a channel. Allows you to control whether or not the client may create the desired channel.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server on which the client attempts to create the channel
	 * @param client a @ref ClientMiniExport describing the client trying to create a channel
	 * @param parentChannelID the channel that is the parent channel of the channel to be created. 0 if the channel to be created will be a root channel.
	 * @param variables a @ref VariablesExport struct that describes the channel to be created.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permChannelCreate)                  (uint64 serverID, const struct ClientMiniExport* client, uint64 parentChannelID, const struct VariablesExport* variables);

	/**
	 * @brief called when a channel is about to be edited by a client. Allows you to prevent channel edits.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client a @ref ClientMiniExport describing the client trying to edit the channel
	 * @param parentChannelID the channel that is to be edited.
	 * @param variables a @ref VariablesExport struct that describes the channel after the edit.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permChannelEdit)                    (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, const struct VariablesExport* variables);

	/**
	 * @brief called before a channel is deleted by a client. Allows you to deny a client deleting channels.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server on which the channel is to be deleted
	 * @param client a @ref ClientMiniExport describing the client trying to delete the channel
	 * @param channelID the channel that is to be deleted
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permChannelDelete)                  (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID);

	/**
	 * @brief called when a client requests to subscribe a channel. Allows you to deny subscribing to a channel.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server on which the client attempts to subscribe to the channel.
	 * @param client a @ref ClientMiniExport describing the client trying to subscribe the channel
	 * @param channelID the channel that is to be subscribed
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permChannelSubscribe)               (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID);

	/**
	 * @brief called when a file is to be uploaded. Allows you to deny a client from uploading files, files above a certain size, etc.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client that attempts to upload the file.
	 * @param params describes the file to be uploaded.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferInitUpload)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftinitupload* params);

	/**
	 * @brief called when a file is to be downloaded. Allows you to deny a client from downloading files, files above a certain size, etc.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client that attempts to download the file
	 * @param params describes the file to be downloaded.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferInitDownload)       (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftinitdownload* params);

	/**
	 * @brief called when a client requests file information using @ref ts3client_requestFileInfo. Allows to deny clients from getting file information.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client attempting to get information of the file.
	 * @param params describes the file that information is requested for.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferGetFileInfo)        (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftgetfileinfo* params);

	/**
	 * @brief called when a client requests a directory listing using @ref ts3client_requestFileList. Allows to deny listing files and directories in channels / directories.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferGetFileList)        (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftgetfilelist* params);

	/**
	 * @brief called when a client attempts to delete one or more files using @ref ts3client_requestDeleteFile. Allows denying clients deleting files.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client attempting to delete the file
	 * @param params describes the file to be deleted
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferDeleteFile)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftdeletefile* params);

	/**
	 * @brief called when a directory is to be created using @ref ts3client_requestCreateDirectory. Allows to deny creating certain directories.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client attempting to create the directory
	 * @param params describes the directory to create.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferCreateDirectory)    (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftcreatedir* params);

	/**
	 * @brief called when a file is to be renamed or moved using @ref ts3client_requestRenameFile. Allows to deny moving files or even renaming files.
	 *
	 * Return @ref ERROR_ok to allow the action, or @ref ERROR_permissions to reject it.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client attempting to rename or move the file.
	 * @param params describes the file to be renamed or moved, and where the file should be moved to if it's being moved.
	 * @return @ref ERROR_ok to allow, @ref ERROR_permissions to deny
	*/
	unsigned int (*permFileTransferRenameFile)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftrenamefile* params);

	/**
	 * @brief called when a server or channel password is set.
	 *
	 * Used to hash the password or encrypt it for check with outside sources.
	 *
	 * @param serverID the server for which the callback was called
	 * @param plaintext the plaintext password to be encrypted.
	 * @param encryptedText the encrypted/hashed password. Fill with your encrypted password. Must be an utf8 encoded c string not larger than specified by encryptedTextByteSize
	 * @param encryptedTextByteSize the maximum number of bytes you may write to encryptedText
	*/
	void (*onClientPasswordEncrypt)                    (uint64 serverID, const char* plaintext, char* encryptedText, int encryptedTextByteSize);

	/**
	 * @brief Allows to rewrite the file path and name of the file to be transfered. Called when a transfer starts.
	 * @details If you don't need to control server side file name and path then don't implement this callback.
	 * The parameters are already filled with the default values intended by the client starting the transfer. These can be changed as required.
	 * When the callback exits with @ref ERROR_ok the transfer is started with the values present in the result struct.
	 * @verbatim embed:rst
	   See the :ref:`SDK documentation <s-rewrite-path>` for further details.
	   @endverbatim
	 *
	 * @param serverID the server for which the callback was called
	 * @param invokerClientID the client which started the file transfer
	 * @param original the original file path and name desired by the client
	 * @param result the values from this struct will be used by the server when the callback exits. Already filled with a copy of original. Change the values in this struct as needed.
	 * @return a value from the @ref Ts3ErrorType enum. Return @ref ERROR_ok to start the transfer with the values in the result struct. When returning an error code the file transfer is not started.
	*/
	unsigned int (*onTransformFilePath)                (uint64 serverID, anyID invokerClientID, const struct TransformFilePathExport* original, struct TransformFilePathExportReturns* result);

	/**
	 * @brief called when a client connects to the server. Used to verify the server password when using custom password encryption.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client that connects to the server
	 * @param password utf8 encoded c string containing the password provided by the client.
	 * @return a value from the @ref Ts3ErrorType enum. @ref ERROR_ok if the password is valid, @ref ERROR_server_invalid_password if the password is not valid, @ref ERROR_parameter_invalid if the password is in invalid format.
	*/
	unsigned int (*onCustomServerPasswordCheck)        (uint64 serverID, const struct ClientMiniExport* client, const char* password);

	/**
	 * @brief called when a client attempts to enter a password protected channel. Used to verify the channel password when using custom password encryption.
	 *
	 * @param serverID the server for which the callback was called
	 * @param client describes the client that enters a channel
	 * @param channelID the channel the client attempts to join
	 * @param password utf8 encoded c string containing the password provided by the client.
	 * @return a value from the @ref Ts3ErrorType enum. @ref ERROR_ok if the password is valid, @ref ERROR_server_invalid_password if the password is not valid, @ref ERROR_parameter_invalid if the password is in invalid format.
	*/
	unsigned int (*onCustomChannelPasswordCheck)       (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, const char* password);

}; //END OF ServerLibFunctions

/*Memory management*/

/**
 * @brief Releases memory allocated by the server library
 *
 * For every function that has output parameters which take pointers to memory (e.g. char**)
 * the server library will allocate sufficient memory for you, however you need to take care
 * of releasing the memory by passing the variable to this function.
 *
 * @param pointer pointer to memory allocated by server library
*/
EXPORTDLL unsigned int ts3server_freeMemory(void* pointer);

/*Construction and Destruction*/

/**
 * @brief initializes the server library and defines callback functions
 *
 * This is the first function you need to call, before this all calls to the server library will fail. In this call you will also set the functions you would like to have called when
 * certain changes or events happen. This function must not be called multiple times.
 *
 * @param functionPointers defines which functions in your code are to be called on specific events. Zero initialize it and assign the desired function to call to the respective members of the @ref ServerLibFunctions struct
 * @param usedLogTypes a combination of values from the @ref LogTypes enum. Specifies which type(s) of logging you would like to use.
 * @param logFileFolder path in which to create log files. Pass 0 to use the default of using a folder called logs in the working directory.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_initServerLib(const struct ServerLibFunctions* functionPointers, int usedLogTypes, const char* logFileFolder);

/**
 * @brief Initialize the file transfer subsystem. Allows clients to store files on the machine the server is running on and download them.
 *
 * If you want to support file transfer functionality, then call this function after calling @ref ts3server_initServerLib
 * If you don't call this function file transfer features will not be available.
 * The server library will create the directories necessary for storing files as needed, however directories will not be cleaned up by
 * the server library. Instead it is the responsibility of the application to clean up these directories when they're no longer needed (e.g. after a virtual server was deleted)
 *
 * @param filebase path to where the server library will create necessary directories and store files uploaded by clients.
 * @param ips zero terminated array of IP addresses to listen on for file transfer connections. IPv4 and IPv6 addresses are supported, do NOT pass host names.
 * If set to 0, it will be treated as if you passed { "0.0.0.0", "::", 0 }
 * @param port the TCP port to listen on for file transfer connections.
 * @param downloadBandwidth limit in bytes per second which is available for downloading files from the server. Speed across all transfers will be limited to this number. Specify @ref BANDWIDTH_LIMIT_UNLIMITED for no limit.
 * @param uploadBandwidth limit in bytes per second which is available for uploading files to the server. Speed across all transfers will be limited to this number. Specify @ref BANDWIDTH_LIMIT_UNLIMITED for no limit.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_enableFileManager(const char* filebase, const char** ips, int port, uint64 downloadBandwidth, uint64 uploadBandwidth);

/**
 * @brief Destroys the server lib. Must not be called from within a callback.
 *
 * All clients will lose connection and timeout, all servers will terminate.
 * This is the last function to call. After this call you will no longer be able to use any server library functions.
 *
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_destroyServerLib();

/**
 * @brief Prevents clients from performing certain actions. SDK only.
 *
 * Use this to disable certain features for clients, e.g. deleting channels or moving clients
 * so that the server has authority over these matters and is the only entity who can do so.
 * To disable multiple commands, call this function once for each command you would like to disable for clients.
 *
 * @param clientCommand the command to disable. One of the values from the @ref ClientCommand enum
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_disableClientCommand(int clientCommand);

/**
 * @brief Retrieve the server version string
 *
 * @param result address of a variable to receive the server version.
 * Memory is allocated by the server library and must be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getServerLibVersion(char** result);

/**
 * @brief Retrieve the server version number
 *
 * @param result address of a variable to receive the server version number.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getServerLibVersionNumber(uint64* result);

/*logging*/

/**
 * @brief Specify which log messages to send to the @ref ServerLibFunctions.onUserLoggingMessageEvent callback
 *
 * @param logVerbosity Minimum verbosity of a log message to pass to the callback. One of the values from the @ref LogLevel enum.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/

/**
 * @brief get a human readable error description string for an error code
 *
 * @param globalErrorCode the error code to retrieve the description for. One of the values from the @ref Ts3ErrorType enum.
 * @param result address of a variable to receive the error description as a utf8 encoded c string.
 * Memory is allocated by the server library and must be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getGlobalErrorMessage(unsigned int globalErrorCode, char** result);

/*client info*/

/**
 * @brief get the value of a client variable as integer.
 *
 * Not all variables are available as integer, some are only available as string or unsigned 64 bit integer.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param result address of a variable to receive the value of the variable queried.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int* result);

/**
 * @brief get the value of a client variable as unsigned 64 bit integer.
 *
 * Not all variables are available as unsigned 64 bit integer, some are only available as string or integer.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param result address of a variable to receive the value of the variable queried.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getClientVariableAsUInt64(uint64 serverID, anyID clientID, enum ClientProperties flag, uint64* result);

/**
 * @brief get the value of the client variable as string
 *
 * Not all variables are available as string, some are only available as unsigned 64 bit integer or integer.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param result address of a variable to receive the value of the variable queried.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, char** result);

/**
 * @brief set the value of a client variable.
 *
 * Not all variables can be set as integer.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int value);

/**
 * @brief set the value of a client variable.
 *
 * Not all variables can be set as unsigned 64 bit integer.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setClientVariableAsUInt64(uint64 serverID, anyID clientID, enum ClientProperties flag, uint64 value);

/**
 * @brief set the value of a client variable.
 *
 * Not all variables can be set as string.
 *
 * @param serverID specifies the server the client is on
 * @param clientID which client to query
 * @param flag specifies which variable to retrieve. One of the values from the @ref ClientProperties enum.
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, const char* value);

/**
 * @brief Apply and publish client changes after setting client variables.
 *
 * @param serverID specifies the server the client is on
 * @param clientID the client which we have changed variables for.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_flushClientVariable(uint64 serverID, anyID clientID);

/**
 * @brief set a clients whisper list. Will stop transmitting that clients voice to their current channel.
 *
 * The client will still receive voice from their current channel, however their voice will not be transmitted to their current channel anymore.
 * The voice data of the specified client will be transmitted to all specified channels and all the specified clients.
 * Pass 0 to both channelID and clientID to restore default behavior of transmitting voice to current channel.
 *
 * @param serverID the server on which to set the whisper list
 * @param clID the client for which to set the whisper list
 * @param channelID zero terminated array of channel ids to add to the whisper list. Pass nullptr to reset. Like { 3, 94, 84, ..., 0 }
 * @param clientID zero terminated array of client ids to add to the whisper list. Pass nullptr to reset. Like { 1, 4, ..., 0 }
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setClientWhisperList(uint64 serverID, anyID clID, const uint64* channelID, const anyID* clientID);

/**
 * @brief get a list of all clients connected to a server
 *
 * @param serverID specifies the server on which to get the list of clients
 * @param result address of a variable to receive the zero terminated list of clients, like {1, 2, 50, ..., 0}
 * Memory is allocated by the server library and caller must free the array using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getClientList(uint64 serverID, anyID** result);

/**
 * @brief get the id of the clients current channel
 *
 * @param serverID specifies the server the client is on
 * @param clientID the client to get the channel of
 * @param result address of a variable to receive the channel id
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelOfClient(uint64 serverID, anyID clientID, uint64* result);

/**
 * @brief Move one or more clients to a different channel
 *
 * @param serverID specifies the server the client is connected to
 * @param newChannelID the id of the channel to move the client(s) to
 * @param clientIDArray zero terminated array of client ids to move. Like {4, 9, ..., 0}
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_clientMove(uint64 serverID, uint64 newChannelID, const anyID* clientIDArray);

/**
 * @brief kick one or more clients from the server, terminating their connection.
 *
 * @param serverID the server the client(s) are connected to
 * @param clientIDArray zero terminated array of client ids to kick. Like {4, 3, 12, ..., 0}
 * @param kickReason utf8 encoded c string describing the reason for the kick. Pass an empty string if unused.
 * @param failOnClientError boolean flag. If 1 the function will fail if clients to be kicked are not on the server.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_clientsKickFromServer(uint64 serverID, const anyID* clientIDArray, const char* kickReason, int failOnClientError);

/**
 * @brief get a list of clients that are using one of the specified public identities
 *
 * @param serverID the server to check for clients on
 * @param clientUIDs address of a zero terminated array containing the client unique identifiers to find client ids for. Like { "uid1", "uid2", ..., '@0' }
 * @param result address of a variable to receive the client ids using any of the supplied unique identifiers.
 * Memory is allocated by the server library and caller must free the array using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getClientIDSfromUIDS(uint64 serverID, const char** clientUIDs, anyID** result);

/*channel info*/

/**
 * @brief get value of a channel variable as integer.
 *
 * Not all variables are available as integer, some are only available as string or unsigned 64 bit integer.
 *
 * @param serverID specifies the server the channel is located on
 * @param channelID the id of the channel to get the variable for
 * @param flag specifies which variable to retrieve. One of the values from the @ref ChannelProperties enum
 * @param result address of a variable to receive the value of the queried variable.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int* result);

/**
 * @brief get value of a channel variable as unsigned 64 bit integer.
 *
 * Not all variables are available as unsigned 64 bit integer, some are only available as string or integer.
 *
 * @param serverID specifies the server the channel is located on
 * @param channelID the id of the channel to get the variable for
 * @param flag specifies which variable to retrieve. One of the values from the @ref ChannelProperties enum
 * @param result address of a variable to receive the value of the queried variable.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsUInt64(uint64 serverID, uint64 channelID, enum ChannelProperties flag, uint64* result);

/**
 * @brief get value of a channel variable as string.
 *
 * Not all variables are available as string, some are only available as integer or unsigned 64 bit integer.
 *
 * @param serverID specifies the server the channel is located on
 * @param channelID the id of the channel to get the variable for
 * @param flag specifies which variable to retrieve. One of the values from the @ref ChannelProperties enum
 * @param result address of a variable to receive the value of the queried variable.
 * Memory is allocated by the server library and caller must free it using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, char** result);

/**
 * @brief set the variable of a channel to a new value.
 *
 * Call @ref ts3server_flushChannelVariable after having set all variables you need to change.
 *
 * @param serverID specifies the server the channel is located on
 * @param channelID specifies the channel on which to change the variable
 * @param flag specifies which variable to change. One of the values from the @ref ChannelProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int value);

/**
 * @brief set a channel variable
 *
 * Call @ref ts3server_flushChannelVariable after having set all variables you need to change.
 *
 * @param serverID the server on which the channel is located
 * @param channelID the id of the channel to set the variable for
 * @param flag specifies which variable to set. One of the values from the @ref ChannelProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setChannelVariableAsUInt64(uint64 serverID, uint64 channelID, enum ChannelProperties flag, uint64 value);

/**
 * @brief
 *
 * Call @ref ts3server_flushChannelVariable after having set all variables you need to change.
 *
 * @param serverID the server on which the channel is located
 * @param channelID the id of the channel to set the variable for
 * @param flag specifies which variable to set. One of the values from the @ref ChannelProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, const char* value);

/**
 * @brief After changing channel variables call this function to publish the changes to connected clients
 *
 * @param serverID the server on which channels were edited
 * @param channelID the channel which variables were changed
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_flushChannelVariable(uint64 serverID, uint64 channelID);

/**
 * @brief After setting the channel properties on a new channel, call this function to publish the channel to clients
 *
 * @param serverID the server on which to create the channel
 * @param channelParentID the id of the parent channel for the new channel
 * @param result address of a variable to receive the channel id of the newly created channel
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_flushChannelCreation(uint64 serverID, uint64 channelParentID, uint64* result);

/**
 * @brief Create a structure that defines channel properties for use with @ref ts3server_createChannel.
 *
 * @param result address of a variable to receive a pointer to the structure. Pointer must not be freed!
 * After receiving the structure pointer, it must be filled using @ref ts3server_setChannelCreationParams
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_makeChannelCreationParams(struct TS3ChannelCreationParams** result);

/**
 * @brief Specify mandatory details of a channel to be created at server creation using @ref ts3server_createVirtualServer2
 *
 * @details Must be called after @ref ts3server_getVirtualServerCreationParamsChannelCreationParams to set basic properties of a channel.
 * After this call you may set additional channel properties by calling @ref ts3server_getChannelCreationParamsVariables and @ref ts3server_setVariableAsInt, @ref ts3server_setVariableAsUInt64 or @ref ts3server_setVariableAsString
 *
 * @param channelCreationParams defines the channel for which we set basic properties. Obtained by calling @ref ts3server_getVirtualServerCreationParamsChannelCreationParams
 * @param channelParentID the id of the channel that this channel is a sub channel of. Pass 0 to make this channel a root channel.
 * @param channelID the id this channel should have. Pass 0 to have the server lib assign a free ID. This is used to identify the channel in other calls to the client and server library. Must be unique across all virtual servers during the lifetime of the server library.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setChannelCreationParams(struct TS3ChannelCreationParams* channelCreationParams, uint64 channelParentID, uint64 channelID); /*0 id's means undefined*/

/**
 * @brief Allows settings optional channel properties for channels to be created either at server creation using @ref ts3server_createVirtualServer2 or using @ref ts3server_createChannel.
 *
 * @param channelCreationParams defines which channel to set variables for. Address of the structure obtained by calling @ref ts3server_getVirtualServerCreationParamsChannelCreationParams
 * when the channel is to be created at server creation (using @ref ts3server_createVirtualServer2) or obtained by calling @ref ts3server_makeChannelCreationParams when using with @ref ts3server_createChannel
 * @param result address of a variable to receive a pointer that is to be filled using @ref ts3server_setVariableAsInt, @ref ts3server_setVariableAsUInt64 or @ref ts3server_setVariableAsString
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelCreationParamsVariables(struct TS3ChannelCreationParams* channelCreationParams, struct TS3Variables** result);

/**
 * @brief create a new channel on an existing virtual server.
 *
 * @param serverID the server on which to create the channel.
 * @param channelCreationParams defines channel properties. Address of the structure obtained by calling @ref ts3server_makeChannelCreationParams
 * Must have been filled using @ref ts3server_setChannelCreationParams before this call.
 * @param flags defines how certain information is presented in the channelCreationParams. Combination of the values from the @ref ChannelCreateFlags enum
 * @param result address of a variable to receive the channel id of the newly created channel.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_createChannel(uint64 serverID, struct TS3ChannelCreationParams* channelCreationParams, enum ChannelCreateFlags flags, uint64* result);

/**
 * @brief list all channels on the server
 *
 * @param serverID the server to get the list of channels on
 * @param result address of a variable to receive a zero terminted array of channel ids. Like {4, 65, 23, ..., 0}
 * Memory is allocated by the server library and caller must free the array using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelList(uint64 serverID, uint64** result);

/**
 * @brief get list of clients in a channel
 *
 * @param serverID the server on which the channel is located
 * @param channelID the channel of which to get the list of clients
 * @param result address of a variable to receive a zero terminated array of client ids in the channel. Like {3, 5, 39, ..., 0}
 * Memory is allocated by the server library and caller must free the array using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getChannelClientList(uint64 serverID, uint64 channelID, anyID** result);

/**
 * @brief get the parent channel of a channel
 *
 * @param serverID the server on which the channel is located
 * @param channelID the channel of which to get the parent channel
 * @param result address of a variable to receive the parent channel id
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getParentChannelOfChannel(uint64 serverID, uint64 channelID, uint64* result);

/**
 * @brief delete a channel
 *
 * @param serverID the server on which the channel is located
 * @param channelID the id of the channel to delete
 * @param force boolean flag, 1 = delete even if there are clients or sub channels in the channel. 0 = fail if there are sub channels or clients in the channel or sub channels.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_channelDelete(uint64 serverID, uint64 channelID, int force);

/**
 * @brief move a channel within the tree, make it a sub channel or root channel.
 *
 * @param serverID the server on which to move a channel
 * @param channelID the channel to move
 * @param newChannelParentID id of the parent channel to move this channel into. Set to 0 to make this channel a root channel.
 * @param newOrder id of the channel below which this channel is to be sorted.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_channelMove(uint64 serverID, uint64 channelID, uint64 newChannelParentID, uint64 newOrder);

/*server info*/

/**
 * @brief get the value of a server variable
 *
 * @param serverID the server of which to get a variable value
 * @param flag specifies for which variable to get the value. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive the result
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int* result);

/**
 * @brief get the value of a server variable
 *
 * @param serverID the server of which to get a variable value
 * @param flag specifies for which variable to get the value. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive the result.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsUInt64(uint64 serverID, enum VirtualServerProperties flag, uint64* result);

/**
 * @brief get the value of a server variable
 *
 * @param serverID the server of which to get a variable value
 * @param flag specifies for which variable to get the value. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive a utf8 encoded c string containing the value.
 * Memory is allocated by the server library and must be freed by the caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, char** result);

/**
 * @brief set a new value for a server variable
 *
 * After you're done setting all the variables you need to change, a call to @ref ts3server_flushVirtualServerVariable is necessary to publish the changes
 *
 * @param serverID specifies which server to set the variable on
 * @param flag specifies which server variable to set. One of the values from the @ref VirtualServerProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int value);

/**
 * @brief set a new value for a server variable
 *
 * After you're done setting all the variables you need to change, a call to @ref ts3server_flushVirtualServerVariable is necessary to publish the changes
 *
 * @param serverID specifies which server to set the variable on
 * @param flag specifies which server variable to set. One of the values from the @ref VirtualServerProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsUInt64(uint64 serverID, enum VirtualServerProperties flag, uint64 value);

/**
 * @brief set a new value for a server variable
 *
 * After you're done setting all the variables you need to change, a call to @ref ts3server_flushVirtualServerVariable is necessary to publish the changes
 *
 * @param serverID specifies which server to set the variable on
 * @param flag specifies which server variable to set. One of the values from the @ref VirtualServerProperties enum
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, const char* value);

/**
 * @brief Publish server changes done through previous calls to @ref ts3server_setVirtualServerVariableAsInt, @ref ts3server_setVirtualServerVariableAsString, @ref ts3server_setVirtualServerVariableAsUInt64
 *
 * @param serverID the server you previously called setVirtualServerVariableAs functions
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_flushVirtualServerVariable(uint64 serverID);

/**
 * @brief Creates a structure to define an entire virtual server including the channel layout for server creation for use with @ref ts3server_createVirtualServer2
 *
 * @details This is the first function to call when using the @ref ts3server_createVirtualServer2 meachanism of creating virtual servers in one go, including all of their channels.
 * After receiving the structure using this function, you need to call @ref ts3server_setVirtualServerCreationParams to set basic configuration for this virtual server.
 * Once that is done you can set additional parameters using @ref ts3server_getVirtualServerCreationParamsVariables and @ref ts3server_setVariableAsInt, @ref ts3server_setVariableAsUInt64 or @ref ts3server_setVariableAsString
 *
 * @param result address of a variable to receive a pointer to a structure defining the virtual server. This must be filled using @ref ts3server_setVirtualServerCreationParams after this.
 * Pointer must not be freed by caller.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_makeVirtualServerCreationParams(struct TS3VirtualServerCreationParams** result);

/**
 * @brief Set mandatory server creation properties for server creation using @ref ts3server_createVirtualServer2.
 *
 * @details This call is mandatory after calling @ref ts3server_makeVirtualServerCreationParams when using @ref ts3server_createVirtualServer2 and sets the basic information to create a virtual server.
 * After this call you can optionally set other variables by calling @ref ts3server_getVirtualServerCreationParamsVariables after this.
 *
 * @param virtualServerCreationParams pointer to a struct of creation parameters obtained by calling @ref ts3server_makeVirtualServerCreationParams
 * @param serverPort the UDP port to listen for client connections on
 * @param serverIp comma separated list of IP address(es) to listen for client connections on. IPv4 and IPv6 addresses are supported.
 * @param serverKeyPair unique key for encryption. Pass an empty string when originally creating a new server, query the generated encryption key with @ref ts3server_getVirtualServerKeyPair, store it and use it on subsequent start ups.
 * @param serverMaxClients maximum number of clients that can be connected simultaneously at any given time
 * @param channelCount the amount of channels this server will have after creation. You must call @ref ts3server_getVirtualServerCreationParamsChannelCreationParams with this virtualServerCreationParams exactly this many times.
 * @param serverID the id this virtual server will have when created. server id must be unique during life time of the server library.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVirtualServerCreationParams(struct TS3VirtualServerCreationParams* virtualServerCreationParams, unsigned int serverPort, const char* serverIp, const char* serverKeyPair, unsigned int serverMaxClients, unsigned int channelCount, uint64 serverID);

/**
 * @brief create struct to define optional server settings for server creation with @ref ts3server_createVirtualServer2
 *
 * @param virtualServerCreationParams
 * @param result address of a variable to receive a pointer to a TS3Variables struct, to set optional server settings using @ref ts3server_setVariableAsInt, @ref ts3server_setVariableAsString or @ref ts3server_setVariableAsUInt64.
 * Memory must not be freed by caller.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerCreationParamsVariables(struct TS3VirtualServerCreationParams* virtualServerCreationParams, struct TS3Variables** result);

/**
 * @brief Used to specify channels to create during advanced server creation using @ref ts3server_createVirtualServer2.
 *
 * @details Call this function exactly as often as you indicated channels to be created in the @ref ts3server_setVirtualServerCreationParams call.
 * Once you have received the struct you must set the details using @ref ts3server_setChannelCreationParams and can optionally set additional parameters using
 * @ref ts3server_getChannelCreationParamsVariables to get a structure to fill using @ref ts3server_setVariableAsInt, @ref ts3server_setVariableAsString, @ref ts3server_setVariableAsUInt64
 *
 * @param virtualServerCreationParams pointer to the server structure on which to create this channel. Obtained by calling @ref ts3server_makeVirtualServerCreationParams
 * @param channelIdx specifies the channel to specify creation parameters for. Must be > 0 and < the channelCount parameter specified in the @ref ts3server_setVirtualServerCreationParams call
 * @param result address of a variable to receive a pointer that is to be filled using @ref ts3server_setChannelCreationParams. Must not be freed by caller.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerCreationParamsChannelCreationParams(struct TS3VirtualServerCreationParams* virtualServerCreationParams, unsigned int channelIdx, struct TS3ChannelCreationParams** result);

/**
 * @brief Create an entire server structure in a single call. Useful for restoring an entire virtual server including channels including their ids after storing them on shutdown.
 * See the SDK documentation for more in depth information.
 *
 * @details This requires a few other calls to be made in advance. First you need to call @ref ts3server_makeVirtualServerCreationParams to get a
 * TS3VirtualServerCreationParams struct that then needs to be filled via @ref ts3server_setVirtualServerCreationParams.
 * You can then use @ref ts3server_getVirtualServerCreationParamsVariables to set other server settings and use @ref ts3server_getVirtualServerCreationParamsChannelCreationParams
 * to specify channels to create using @ref ts3server_setChannelCreationParams.
 *
 * @param virtualServerCreationParams pointer to the server parameters obtained by calling @ref ts3server_makeVirtualServerCreationParams. These must have been filled using
 * @ref ts3server_setVirtualServerCreationParams before calling this function.
 * @param flags defines how certain information is present in the virtualServerCreationParams. Combination of the values from the @ref VirtualServerCreateFlags enum
 * @param result address of a variable to receive the created servers id. This is used in other calls to the server library to identify this server.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_createVirtualServer2(struct TS3VirtualServerCreationParams* virtualServerCreationParams, enum VirtualServerCreateFlags flags, uint64* result);

/**
 * @brief get value of server connection properties as unsigned integer.
 *
 * @param serverID which server to get connection properties of
 * @param flag specifies which property to get the value of. One of the values from the @ref ConnectionProperties enum
 * @param result address of a variable to receive the value of the connection property
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsUInt64(uint64 serverID, enum ConnectionProperties flag, uint64* result);

/**
 * @brief get value of server connection properties as double
 *
 * @param serverID which server to get connection properties of
 * @param flag specifies which value to get. One of the values from the @ref ConnectionProperties enum
 * @param result address of a variable to receive the value of the connection property.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsDouble(uint64 serverID, enum ConnectionProperties flag, double* result);

/**
 * @brief get a list of virtual servers in this instance
 *
 * @param result address of a variable to receive a zero terminated array of virtual server ids. Like {4, 8, ..., 0}
 * Memory is allocated by the server library and caller must free the array using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerList(uint64** result);

/**
 * @brief deletes a virtual server. All clients will be disconnected and no more connections are accepted. You need to recreate the server
 * using @ref ts3server_createVirtualServer or @ref ts3server_createVirtualServer2 to make it available again.
 *
 * You may want to save the state of the virtual server if you need persistence.
 *
 * @param serverID specifies which server to stop
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_stopVirtualServer(uint64 serverID);

/**
 * @brief create a new virtual server. The server is started automatically after being created.
 *
 * @param serverPort the UDP port to listen for client connections on
 * @param serverIp comma separated list of IP address(es) to listen for client connections on. IPv4 and IPv6 addresses are supported.
 * @param serverName display name of the server.
 * @param serverKeyPair Key pair for encryption. Must be unique for each virtual server. Pass an empty string when originally creating a new server, query the generated encryption key with @ref ts3server_getVirtualServerKeyPair, store it and use it on subsequent start ups.
 * @param serverMaxClients maximum number of clients that can be connected simultaneously at any given time
 * @param result address of a variable that will receive the virtual server ID that can be used to specify this server in future calls to server library functions.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_createVirtualServer(unsigned int serverPort, const char* serverIp, const char* serverName, const char* serverKeyPair, unsigned int serverMaxClients, uint64* result);

/**
 * @brief retrieve the encryption keys used by the virtual server.
 *
 * Store these and use them on subsequent process startup to recreate this server when calling @ref ts3server_createVirtualServer
 *
 * @param serverID the server for which to get the key pair.
 * @param result address of a variable to receive a utf8 encoded c string containing the key pair.
 * Memory is allocated by the server library and must be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVirtualServerKeyPair(uint64 serverID, char** result);

/*security salt/hash*/

/**
 * @brief Create a security salt to lock channel to identities. See the :ref:SDK Documentation<`channel_security_salt`> on the topic for more in depth explanation.
 *
 * @param options specifies which parameters to include in the security salt. A combination of values from the @ref SecuritySaltOptions enum.
 * @param salt pointer to random data of cryptographic quality.
 * @param saltByteSize number of bytes of random data to use. Larger is better but slower.
 * @param securitySalt address of a variable to receive the security salt.
 * Memory is allocated by the server library and needs to be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_createSecuritySalt(int options, void* salt, int saltByteSize, char** securitySalt);

/**
 * @brief create a hash for a specific client from a security salt to lock an identity to a channel. See the :ref:SDK Documentation<`channel_security_salt`> on the topic for more in depth explanation.
 *
 * @param securitySalt the security salt of a channel as generated by @ref ts3server_createSecuritySalt
 * @param clientUniqueIdentifier public identity of a client to generate a security hash for
 * @param clientNickName nickname of the client to include in the hash if specified by the salt.
 * @param clientMetaData meta data of the client to include in the hash if specified by the salt.
 * @param securityHash address of a variable to receive the security hash.
 * Memory is allocated by the server library and must be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_calculateSecurityHash(const char* securitySalt, const char* clientUniqueIdentifier, const char* clientNickName, const char* clientMetaData, char** securityHash);

/*variable editing*/

/**
 * @brief get the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as integer. Some are only available as string or unsigned 64 bit integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive the desired value
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVariableAsInt(struct TS3Variables* var, int flag, int* result);

/**
 * @brief get the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as unsigned 64 bit integer. Some are only available as string or integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive the desired value
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVariableAsUInt64(struct TS3Variables* var, int flag, uint64* result);

/**
 * @brief get the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as string. Some are only available as unsigned 64 bit integer or integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param result address of a variable to receive the desired value.
 * Memory is allocated by the server library and must be freed by caller using @ref ts3server_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_getVariableAsString(struct TS3Variables* var, int flag, char** result);

/**
 * @brief set the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as integer. Some are only available as string or unsigned 64 bit integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param value the new value to set for the variable.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVariableAsInt(struct TS3Variables* var, int flag, int value);

/**
 * @brief set the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as unsigned 64 bit integer. Some are only available as string or integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param value the new value to set for the variable.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVariableAsUInt64(struct TS3Variables* var, int flag, uint64 value);

/**
 * @brief set the value of a property of a server or channel when using @ref ts3server_createVirtualServer2 or @ref ts3server_createChannel
 *
 * Not all properties are available as string. Some are only available as unsigned 64 bit integer or integer.
 *
 * @param var pointer to a TS3Variables struct obtained by calling @ref ts3server_getVirtualServerCreationParamsVariables or @ref ts3server_getChannelCreationParamsVariables
 * @param flag specifies which variable to receive the value of. One of the values from the @ref VirtualServerProperties enum
 * @param value utf8 encoded c string containing the value to set the variable to
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3server_setVariableAsString(struct TS3Variables* var, int flag, const char* value);

#ifdef __cplusplus
}
#endif

#endif
