/*
 * This is the main header for the TeamSpeak 3 Client SDK. All the functions that are used
 * to communicate with the Teamspeak 3 Client are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3sclient_freeMemory()
 * after finishing with the result. This holds true for all the functions returning strings
 * (so the parameter is "char** result"). Again, the details are all in the documentation.
 */

#ifndef CLIENTLIB_H
#define CLIENTLIB_H

//system
#include <stdlib.h>

//own
#include "teamspeak/public_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ClientUIFunctionsRare;

/**
 * @brief Defines available callbacks that you can receive.
 *
 * Set the members of this struct to a function to call when the specific event happens.
 */
struct ClientUIFunctions {
    /**
     * @brief called when the status of a connection changes
     *
     * @param serverConnectionHandlerID specifies on which connection the status has changed
     * @param newStatus the current status of the connection. One of the values from the ConnectStatus enum
     * @param errorNumber if the state change was caused by an error this is set to one of the values from the @ref Ts3ErrorType enum
    */
    void (*onConnectStatusChangeEvent)              (uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    /**
     * @brief
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
    */
    void (*onServerProtocolVersionEvent)            (uint64 serverConnectionHandlerID, int protocolVersion);

    /**
     * @brief called when a channel was received.
     *
     * Will be called once for every channel during connection initialization. Tells you which channels exist.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the id of the channel
     * @param channelParentID the id of the parent channel. 0 if the channel is a root channel.
    */
    void (*onNewChannelEvent)                       (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID);

    /**
     * @brief called when a new channel was created
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the id of the new channel
     * @param channelParentID the id of the parent channel for the newly created channel. 0 if the channel is a root channel.
    */
    void (*onNewChannelCreatedEvent)                (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);

    /**
     * @brief called when a channel is deleted
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the id of the channel that is deleted. This channel is gone already when this is called. It's not possible to get any information about this channel anymore.
     * @param invokerID client id of the client that deleted the channel. 0 if deleted by the server.
     * @param invokerName utf8 encoded c string containing the display name of the client that caused deletion
     * @param invokerUnqiueIdentifier utf8 encoded c string containing the unique identifier of the client that caused deletion
    */
    void (*onDelChannelEvent)                       (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);

    /**
     * @brief called when a channel is moved to a different location on the server
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the id of the channel being moved
     * @param newChannelParentID the id of the new parent channel
     * @param invokerID client if of the client that moved the channel. 0 if caused by server.
     * @param invokerName utf8 encoded c string containing the display name of the client that moved the channel
     * @param invokerUniqueIdentifier utf8 encoded c string containing the unique identifier of the client that moved the channel
    */
    void (*onChannelMoveEvent)                      (uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);

    /**
     * @brief called when new data for a channel was received from the server
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID id of the channel that new information was received for
    */
    void (*onUpdateChannelEvent)                    (uint64 serverConnectionHandlerID, uint64 channelID);

    /**
     * @brief called when a channel was edited on the server
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the id of the channel that was edited
     * @param invokerID client id that edited the channel. 0 if done by the server
     * @param invokerName utf8 encoded c string containing the display name of the client editing the channel
     * @param invokerUniqueIdentifier utf8 encoded c string containing the uid of the client that edited the channel
    */
    void (*onUpdateChannelEditedEvent)              (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);

    /**
     * @brief called whenever a change for a client is received from the server.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID specifies the client for which variables have changed or are now available
     * @param invokerID the source client that caused the update
     * @param invokerName utf8 encoded c string containing the display name of the client causing the update
     * @param invokerUniqueIdentifier utf8 encoded c string containing the public identity of the client causing the update
    */
    void (*onUpdateClientEvent)                     (uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);

    /**
     * @brief called when a client moves to a different channel, disconnects, connects, gets kicked or banned.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client changing channels
     * @param oldChannelID id of the previous channel of the client.
     * @param newChannelID id of the current channel of the client. Can be 0, if the client disconnected / got kicked / banned.
    */
    void (*onClientMoveEvent)                       (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);

    /**
     * @brief called after subscribing to or unsubscribing from a channel. Called once for every client that is in the (un)subscribed channel at this time.
     *
     * Informs you about newly visible clients after subribing to a channel.
     * Informs about clients that we will no longer receive information about.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client
     * @param oldChannelID id of the channel that the client was in last time we saw the client.
     * @param newChannelID id of the channel the client is currently in.
     * @param visibility whether we can see the client or not. One of the values from the Visibility enum.
     * Allows to distinguish whether this callback was called after a subscribe or unsubscribe.
    */
    void (*onClientMoveSubscriptionEvent)           (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);

    /**
     * @brief called when a client loses connection and times out.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client that lost connection
     * @param oldChannelID channel the client used to be in
     * @param newChannelID always 0
     * @param visibility whether we can see the client. One of the values from the Visibility enum.
     * @param timeoutMessage uft8 encoded c string containing the reason message.
    */
    void (*onClientMoveTimeoutEvent)                (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage);

    /**
     * @brief called when a client was moved by the server or another client
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID the client that was moved
     * @param oldChannelID id of the previous channel the client used to be in
     * @param newChannelID id of the current channel the client was moved to
     * @param visibility whether we can see the client. One of the values from the Visibility enum.
     * @param moverID id of the client that moved the client
     * @param moverName utf8 encoded c string containing the display name of the client that caused the move
     * @param moverUniqueIdentifier utf8 encoded c string containing the identifier of the client that caused the move
     * @param moveMessage utf8 encoded c string containing the reason message
    */
    void (*onClientMoveMovedEvent)                  (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage);

    /**
     * @brief called when a client is kicked from their channel
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client that was kicked
     * @param oldChannelID id of the previous channel the client used to be in
     * @param newChannelID id of the current channel the client was kicked to. This is the id of the server default channel.
     * @param visibility whether we can see the client. One of the values from the Visibility enum.
     * @param kickerID id of the client that kicked the client. 0 if the server kicked the client.
     * @param kickerName utf8 encoded c string containing the display name of the client initiating the kick
     * @param kickerUniqueIdentifier utf8 encoded c string containing the identifier of the client initiating the kick
     * @param kickMessage utf8 encoded c string containing the provided reason for the kick
    */
    void (*onClientKickFromChannelEvent)            (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);

    /**
     * @brief called when a client was kicked from the server
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client that was kicked
     * @param oldChannelID id of the previous channel the client used to be in
     * @param newChannelID always 0
     * @param visibility whether we can see the client. One of the values from the Visibility enum.
     * @param kickerID id of the client that kicked the client. 0 if the server kicked the client.
     * @param kickerName utf8 encoded c string containing the display name of the client initiating the kick
     * @param kickerUniqueIdentifier utf8 encoded c string containing the identifier of the client initiating the kick
     * @param kickMessage utf8 encoded c string containing the provided reason for the kick
    */
    void (*onClientKickFromServerEvent)             (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);

    /**
     * @brief called for every connection using the identity after a call to ts3client_requestClientIDs.
     *
     * This is called multiple times for each identity queried. Once
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param uniqueClientIdentifier the public identity queried and used by the client
     * @param clientID the id assigned to this client
     * @param clientName the display name of this client
    */
    void (*onClientIDsEvent)                        (uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName);

    /**
     * @brief called after onClientIDsEvent was called for every client using the queried identity.
     *
     * Once this callback is called, you know of all clients on the server that use the identity.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
    */
    void (*onClientIDsFinishedEvent)                (uint64 serverConnectionHandlerID);

    /**
     * @brief called when the server was edited
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param editerID id of the client that edited the server
     * @param editerName utf8 encoded c string containing the display name of the client editing the server
     * @param editerUniqueIdentifier utf8 encoded c string containing the public identity of the client
    */
    void (*onServerEditedEvent)                     (uint64 serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier);

    /**
     * @brief called whenever updates about changed server properties are received from the server.
     *
     * Happens after a call to ts3client_requestServerVariables but can also be called sporadically.
     *
     * @param serverConnectionHandlerID specifies on which connection the updated variables are available
    */
    void (*onServerUpdatedEvent)                    (uint64 serverConnectionHandlerID);

    /**
     * @brief called after an action was performed by us. Tells whether the action was successful or which error occurred.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param errorMessage utf8 encoded c string describing the error
     * @param error the error code the action finished with. One of the values from the @ref Ts3ErrorType enum.
     * @param returnCode a c string identifying the action that caused this error. This is the same string given as returnCode to function calls that request an action on the server
     * @param extraMessage utf8 encoded c string containing additional information if available.
    */
    void (*onServerErrorEvent)                      (uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);

    /**
     * @brief called when the server was stopped
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param shutdownMessage utf8 encoded c string containing the provided reason for the shutdown
    */
    void (*onServerStopEvent)                       (uint64 serverConnectionHandlerID, const char* shutdownMessage);

    /**
     * @brief called when a text message was received
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param targetMode identifies the type of the message. One of the values from the TextMessageTargetMode enum.
     * @param toID the id of the recipient. Depends on the value of targetMode. a channel id for channel chat, own client id for private messages, 0 for server messages
     * @param fromID id of the client that sent the message
     * @param fromName utf8 encoded c string containing the display name of the client sending the message
     * @param fromUniqueIdentifier utf8 encoded c string containing the public identity of the sending client
     * @param message utf8 encoded c string containing the actual message
    */
    void (*onTextMessageEvent)                      (uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message);

    /**
     * @brief called when a client starts or stops talking.
     *
     * This event is only received for clients in our own channel and clients that whisper us
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param status Whether the client is talking or not. One of the values from the TalkStatus enum.
     * @param clientID the client the event was called for
    */
    void (*onTalkStatusChangeEvent)                 (uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);

    /**
     * @brief called when someone whispers us that is not on the list of clients we accept whispers from.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client that tried to whisper us
     * @sa ts3client_allowWhispersFrom
     * @sa ts3client_setWhisperReceiveWhitelist
    */
    void (*onIgnoredWhisperEvent)                   (uint64 serverConnectionHandlerID, anyID clientID);

    /**
     * @brief called when updated connection properties for a client are available.
     *
     * This happens after a call to ts3client_requestConnectionInfo
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client that updated properties are available for
    */
    void (*onConnectionInfoEvent)                   (uint64 serverConnectionHandlerID, anyID clientID);

    /**
     * @brief called after a call ts3client_requestServerConnectionInfo when the connection information for the server are available.
     *
     * Information can now be queried using ts3client_getServerConnectionVariableAsFloat and ts3client_getServerConnectionVariableAsUInt64
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
    */
    void (*onServerConnectionInfoEvent)             (uint64 serverConnectionHandlerID);

    /**
     * @brief called when a channel was successfully subscribed by us
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID id of the channel we subscribed to
    */
    void (*onChannelSubscribeEvent)                 (uint64 serverConnectionHandlerID, uint64 channelID);

    /**
     * @brief called after all channels we attempted to subscribe to are subscribed.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
    */
    void (*onChannelSubscribeFinishedEvent)         (uint64 serverConnectionHandlerID);

    /**
     * @brief called after we unsubscribed from a channel
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID id of the channel we unsubscribed from. Will no longer receive updates about clients in this channel.
    */
    void (*onChannelUnsubscribeEvent)               (uint64 serverConnectionHandlerID, uint64 channelID);

    /**
     * @brief called after all channels we attempted to unsubscribe from are unsubscribed
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
    */
    void (*onChannelUnsubscribeFinishedEvent)       (uint64 serverConnectionHandlerID);

    /**
     * @brief called when the channel description of a channel has changed.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the channel for which the description has changed
    */
    void (*onChannelDescriptionUpdateEvent)         (uint64 serverConnectionHandlerID, uint64 channelID);

    /**
     * @brief called when a channel password was changed. Can be used to invalidate cached passwords
     * previously stored for the channel.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID id of the channel the password was changed on
    */
    void (*onChannelPasswordChangedEvent)           (uint64 serverConnectionHandlerID, uint64 channelID);

    /**
     * @brief called once the playback device was closed on a connection
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @sa ts3client_initiateGracefulPlaybackShutdown
     * @sa ts3client_closePlaybackDevice
    */
    void (*onPlaybackShutdownCompleteEvent)         (uint64 serverConnectionHandlerID);

    /**
     * @brief called when the available devices changed
     *
     * @param modeID utf8 encoded c string describing the mode of the device
     * @param playOrCap indicates whether the device is a capture or playback device
    */
    void (*onSoundDeviceListChangedEvent)           (const char* modeID, int playOrCap);

    /**
     * @brief called before any effects are applied, allows access to individual client raw audio data
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the source client for the audio
     * @param samples buffer of audio data for the client as 16 bit signed at 48kHz
     * @param sampleCount how many audio frames are available in the buffer
     * @param channels number of audio channels in the audio data
    */
    void (*onEditPlaybackVoiceDataEvent)            (uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

    /**
     * @brief called before audio data is mixed together into a single audio stream for playback, but after effects (3D positioning for example) have been applied.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the source client for the audio
     * @param samples buffer of audio data for the client as 16 bit signed at 48kHz
     * @param sampleCount how many audio frames are available in the buffer
     * @param channels number of audio channels in the audio data
     * @param channelSpeakerArray Array with an entry for each channel in the buffer, defining the speaker each channel represents. see SPEAKER_* defines in public_definitions.h
     * @param channelFillMask a bit mask of SPEAKER_* that defines which of the channels in the buffer have audio data. Be sure to set the corresponding flag when adding audio to previously empty channels in the buffer.
    */
    void (*onEditPostProcessVoiceDataEvent)         (uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);

    /**
     * @brief called after mixing individual client audio together but before sending it to playback device.
     *
     * Last chance to access/modify audio data before it gets sent to the playback device.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param samples buffer of audio data as 16 bit signed at 48kHz
     * @param sampleCount how many audio frames are available in the buffer
     * @param channels how many audio channels are available in the buffer
     * @param channelSpeakerArray Array with an entry for each channel in the buffer, defining the speaker each channel represents. See SPEAKER_* defines in public_definitions.h
     * @param channelFillMask a bit mask of SPEAKER_* that defines which of the channels in the buffer have audio data.
    */
    void (*onEditMixedPlaybackVoiceDataEvent)       (uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);

    /**
     * @brief called after audio data was aquired from the capture device, without any pre processing applied. Allows access to raw audio data.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param samples buffer of audio data
     * @param sampleCount how many audio frames are available in the buffer
     * @param channels how many audio channels are available in the buffer
     * @param flags allows to mute the audio stream, set LSB to 1 to mute the audio.
    */
    void (*onEditCapturedVoiceDataPreprocessEvent)  (uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* flags);

    /**
     * @brief called after pre processing has been applied to recorded voice data, before it is sent to the server.
     *
     * This allows access to or modification of captured data from the recording device.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param samples buffer of audio data as 16 bit signed at 48kHz
     * @param sampleCount how many audio frames are available in the buffer
     * @param channels how many audio channels are available in the buffer
     * @param edited bitMask indicating whether you modified the buffer. Set LSB to 1 if you modified the buffer. Bit 2 indicates whether or not this buffer will be sent to the server.
    */
    void (*onEditCapturedVoiceDataEvent)            (uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited);

    /**
     * @brief called to calculate the volume attenuation for the distance in 3D positioning of clients
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param clientID id of the client for which the position is calculated
     * @param distance the distance from own client to the client
     * @param volume the volume calculated by the client lib. Can be modified in the callback.
    */
    void (*onCustom3dRolloffCalculationClientEvent) (uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume);

    /**
     * @brief called to calculate the volume attenuation for the distance in 3D positioning of wave files
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param waveHandle identifies the wave file to calculate the volume for. A handle previously created with @ref ts3client_playWaveFileHandle
     * @param distance the distance from own client to the source of the wave file
     * @param volume the volume of the wave file calculated by the client lib. Can be modified in the callback.
     * @sa ts3client_playWaveFileHandle
    */
    void (*onCustom3dRolloffCalculationWaveEvent)   (uint64 serverConnectionHandlerID, uint64 waveHandle, float distance, float* volume);

    /**
     * @brief called for every log message if the client lib was initialized with user logging
     *
     * @param logmessage utf8 encoded c string containing the text to log
     * @param logLevel indicates severity of the message. One of the values from the LogLevel enum
     * @param logChannel utf8 encoded c string containing the category this message is logged under
     * @param logID the connection handler this message was logged on
     * @param completeLogString utf8 encoded c string containing the complete log message containing all other parameters for convenience
    */
    void (*onUserLoggingMessageEvent)               (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);

    /**
     * @brief called for every packet to be sent to the server. Used to implement custom cryptography.
     *
     * Only implement if you need custom encryption of network traffic. Replaces default encryption.
     * If implemented Encryption and Decryption must be implemented the same way on both server and client.
     *
     * @param dataToSend pointer to a byte array of data to be encrypted. Must not be freed. Write encrypted data to array.
     * Replace array pointer with pointer to own buffer if you need more space. Need to take care of freeing your own memory yourself.
     * @param sizeOfData pointer to the size of the data array.
    */
    void (*onCustomPacketEncryptEvent)              (char** dataToSend, unsigned int* sizeOfData);

    /**
     * @brief called for every packet received from the server. Used to implement custom cryptography.
     *
     * Only implement if you need custom encryption of network traffic. Replaces default encryption.
     * If implemented Encryption and Decryption must be implemented the same way on both server and client.
     *
     * @param dataReceived pointer to byte array of data to decrypt. Must not be freed. Write decrypted data to the array if large enough.
     * Replace array pointer with pointer to own buffer if decrypted data exceeds the array size. Must take care to free own memory.
     * @param sizeOfData pointer to the size of the data array.
    */
    void (*onCustomPacketDecryptEvent)              (char** dataReceived, unsigned int* dataReceivedSize);

    void (*onProvisioningSlotRequestResultEvent)    (unsigned int error, uint64 requestHandle, const char* connectionKey);

    /**
     * @brief called during the connection initialization, allows to check whether the server identifier is the one you expect.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param ServerUniqueIdentifier utf8 encoded c string containing the server identifier of the server connecting to
     * @param cancelConnect allows to cancel the connection. Set variable pointed to to 1 to abort the connection.
    */
    void (*onCheckServerUniqueIdentifierEvent)      (uint64 serverConnectionHandlerID, const char* ServerUniqueIdentifier, int* cancelConnect);

    /**
     * @brief called when a channel password is set.
     *
     * Can be used to implement custom password checks against external sources (e.g. LDAP).
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param plaintext utf8 encoded c string containing the plaintext password as entered by the user
     * @param encryptedText output parameter. Fill with the encrypted password / password hash. Must be an utf8 encoded c string (zero terminated).
     * Must not be larger than the size specified by the encryptedTextByteSize parameter.
     * @param encryptedTextByteSize the maximum amount of bytes (including trailing zero byte) that may be written to encryptedText parameter
    */
    void (*onClientPasswordEncrypt)                 (uint64 serverConnectionHandlerID, const char* plaintext, char* encryptedText, int encryptedTextByteSize);

    /**
     * @brief called when file transfers finish or terminate with an error
     *
     * @param transferID identifies the file transfer the callback was called for. As created by @ref ts3client_requestFile or @ref ts3client_sendFile
     * @param status indicates success status or error reason. One of the values from the @ref Ts3ErrorType enum.
     * @param statusMessage utf8 encoded c string containing a human readable description of the status message
     * @param remotefileSize size of the file in bytes at the source of the transfer.
     * @param serverConnectionHandlerID specifies the connection the transfer was started on
     * @sa ts3client_sendFile
     * @sa ts3client_requestFile
    */
    void (*onFileTransferStatusEvent)               (anyID transferID, unsigned int status, const char* statusMessage, uint64 remotefileSize, uint64 serverConnectionHandlerID);

    /**
     * @brief called as an answer to ts3client_requestFileList. Called once for every file in the requested path, providing file information.
     *
     * Followed by a onFileList_FinishedEvent callback after this callback was called for the last file in the requested path.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the channel in which the file is located
     * @param path the folder in which this file or directory is located
     * @param name the name of the file or directory this event is called for
     * @param size file size in bytes. 0 if this event describes a directory
     * @param datetime unix timestamp of when this file was last modified
     * @param type whether the entry described is a directory or a file. One of the values from the FileTransferType enum.
     * @param incompleteSize number of bytes that have already been transmitted. If not equal to size then this file is still being transmitted or the transfer was aborted.
     * @param returnCode allows to identify which call to ts3client_requestFileList caused this event to be fired. Same as given to the ts3client_requestFileList call. Can be NULL
    */
    void (*onFileListEvent)                        (uint64 serverConnectionHandlerID, uint64 channelID, const char* path, const char* name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char* returnCode);

    /**
     * @brief called after onFileListEvent was called for all directories / files in a given path.
     *
     * This signifies that you now know of all files and directories in the path requested.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the channel for which the file list is now complete
     * @param path the path within the channel that files and directories were requested for.
    */
    void (*onFileListFinishedEvent)                (uint64 serverConnectionHandlerID, uint64 channelID, const char* path);

    /**
     * @brief called after a call to ts3client_requestFileInfo providing the requested information about a file.
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param channelID the channel in which the file resides
     * @param name utf8 encoded c string containing the absolute path within the channel, including the file / directory name.
     * @param size the size of the file in bytes
     * @param datetime unix timestamp for the last time the file was modified
    */
    void (*onFileInfoEvent)                        (uint64 serverConnectionHandlerID, uint64 channelID, const char* name, uint64 size, uint64 datetime);

    /**
     * @brief called after a call to ts3client_getChatLoginToken providing the requested login token for the chat server associated with this teamspeak server
     * 
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param token THe requested chat login token
     */
    void (*onChatLoginTokenEvent)                  (uint64 serverConnectionHandlerID, const char* token);

    /**
     * @brief called after a call to ts3client_getAuthenticationToken providing the requested authentication token for the virtual server
     *
     * @param serverConnectionHandlerID specifies on which connection the callback was called
     * @param token THe requested authentication token
     */
    void (*onAuthenticationTokenEvent)             (uint64 serverConnectionHandlerID, const char* token);
}; //END OF ClientUIFunctions

/*Memory management*/

/**
 * @brief Releases memory allocated by the client library
 *
 * For every function that has output parameters which take pointers to memory (e.g. char**)
 * the client library will allocate sufficient memory for you, however you need to take care
 * of releasing the memory by passing the variable to this function.
 *
 * @param pointer pointer to memory allocated by the client library
*/
EXPORTDLL unsigned int ts3client_freeMemory(void* pointer);

/*Construction and Destruction*/

/**
 * @brief initializes the client library and defines callback functions
 *
 * This is the first function you need to call, before this all calls to the client library will fail. In this call you will also set the functions you would like to have called when
 * certain changes happen on the client side as well as on connected servers.
 *
 * @param functionPointers defines which functions in your code are to be called on specific events. Zero initialize it and assign the desired function to call to the respective members of the struct
 * @param functionRarePointers similar to the functionPointers parameter. These are not available in the SDK, so SDK users should pass a nullptr here.
 * @param usedLogTypes a combination of values from the @ref LogTypes enum. Specifies which type(s) of logging you would like to use.
 * @param logFileFolder path in which to create log files
 * @param resourcesFolder path to the directory in which the soundbackends folder is located. Required to be able to load the sound backends and process audio.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_initClientLib(const struct ClientUIFunctions* functionPointers, const struct ClientUIFunctionsRare* functionRarePointers, int usedLogTypes, const char* logFileFolder, const char* resourcesFolder);

/**
 * @brief destroys the client library. Must not be called from within a callback.
 *
 * This is the last function to call, after calling this function you will no longer be able to use client library functions.
 *
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_destroyClientLib();

/**
 * @brief Get the version string of the client library
 *
 * @param result Pointer to a char* variable that the client library will allocate memory for.
 *               If the return value is ERROR_ok the memory was allocated and the variable pointed to will contain the client library version.
 *               You need to free the variable pointed to by using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientLibVersion(char** result);

/**
 * @brief Get the version number of the client library
 *
 * @param result Pointer to a variable to store the client library version number into
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientLibVersionNumber(uint64* result);

/**
 * @brief Creates a new server connection handler to connect to servers
 *
 * A connection handler is what handles and identifies server connections to the client library. There can be many of these at the same time
 * and every single one of them can be connected to any server. The client library identifies them by the id placed in the result param.
 * When you receive callbacks, or need to change things, on a specific server you will also specify which server you would like to use by
 * providing the corresponding serverConnectionHandlerId to the client library function.
 *
 * @param port the local port to use. Specify 0 to use an ephemeral port.
 * @param result Address of a variable to store the id of the connection handler in. Use this to reference the connection handler in future calls to client lib functions.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_spawnNewServerConnectionHandler(int port, uint64* result);

/**
 * @brief Destroys a connection handler
 *
 * After destruction the connection handler is invalid and cannot be used any longer. Must not be called from within a callback!
 *
 * @param serverConnectionHandlerID which connection handler to destroy
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_destroyServerConnectionHandler(uint64 serverConnectionHandlerID);

/*Identity management*/

/**
 * @brief Create a new identity to use for connecting to a server
 *
 * Identities identify a client to the server. The identity should be stored and reused for sessions by the same user.
 *
 * @param result Address of a variable to store the identity in.
 * Memory is allocated by the client lib and caller must free it using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_createIdentity(char** result);

/**
 * @brief Get the unique client identifier from an identity
 *
 * @param identityString The identity to produce the unique identifier for, as created by ts3client_createIdentity
 * @param result Pointer to a variable to store the unique client identifier in.
 * Memory is allocated by the client lib and caller must free it using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_identityStringToUniqueIdentifier(const char* identityString, char** result);

/*sound*/

/**
 * @brief Retrieve available playback devices as reported by the operating system
 *
 * @param modeID a string indicating a valid playback mode as retrieved by ts3client_getPlaybackModeList or
 * ts3client_getDefaultPlaybackMode
 * @param result address of a variable that receives a NULL terminated array like `{{char* deviceName, char* deviceId,
 * char* interfaceName char* description, char* fromFactor} ..., NULL}` on windows, `{{char* deviceName, char*
 * deviceId}, ..., NULL}` on other platforms. Memory is allocated by the client lib and caller must free individual strings, array members
 * and the array itself using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 */
EXPORTDLL unsigned int ts3client_getPlaybackDeviceList(const char* modeID, char**** result);

/**
 * @brief Retrieve available recording devices as reported by the operating system
 *
 * @param modeID a string indicating a valid capture mode as retrieved by ts3client_getCaptureModeList or
 * ts3client_getDefaultCaptureMode
 * @param result address of a variable that receives a NULL terminated array like `{{char* deviceName, char* deviceId,
 * char* interfaceName char* description, char* fromFactor}
 * ..., NULL}`  on windows, `{{char* deviceName, char* deviceId}, ..., NULL}` on other platforms.
 * Memory is allocated by the client lib and caller must free individual strings and the array itself using @ref
 * ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 */
EXPORTDLL unsigned int ts3client_getCaptureDeviceList(const char* modeID, char**** result);

/**
 * @brief Retrieve available playback modes
 *
 * @param result address of a variable that receives a NULL terminated array of utf8 encoded c strings of available playback modes.
 * Memory is allocated by the client lib and caller must free individual strings and the array itself using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getPlaybackModeList(char*** result);

/**
 * @brief Retrieve available capture modes
 *
 * @param result address of a variable that receives a NULL terminated array of utf8 encoded c strings of available capture modes.
 * Memory is allocated by the client lib and caller must free individual strings and the array itself using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCaptureModeList(char*** result);

/**
 * @brief Get the current operating system defined default playback device for the indicated mode
 *
 * The operating system may define different devices for different modes.
 *
 * @param modeID a string indicating a valid playback mode as retrieved by @ref ts3client_getPlaybackModeList or @ref ts3client_getDefaultPlayBackMode
 * @param result Address of a variable that receives a NULL terminated array of two c strings like `{char* deviceName, char* deviceID, NULL}`
 * Memory is allocated by the client lib and caller must free individual strings and the array itself using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getDefaultPlaybackDevice(const char* modeID, char*** result);

/**
 * @brief Get the current operating system defined default capture device for the indicated mode
 *
 * The operating system may define different devices for different modes.
 *
 * @param modeID a string indicating a valid capture mode as retrieved by @ref ts3client_getCaptureModeList or @ref ts3client_getDefaultCaptureMode
 * @param result Address of a variable that receives a NULL terminated array of two c strings like `{char* deviceName, char* deviceID, NULL}`
 * Memory is allocated by the client lib and both the array and its individual members must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getDefaultCaptureDevice(const char* modeID, char*** result);

/**
 * @brief Retrieve the current default playback mode
 *
 * @param result Address of a char array to receive the c string indicating the default mode.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getDefaultPlayBackMode(char** result);

/**
 * @brief Retrieve the current default capture mode
 *
 * @param result Address of a char array to receive a c string indicating the default mode.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getDefaultCaptureMode(char** result);

/**
 * @brief initializes a playback device for a connection handler
 *
 * Call this function to start audio playback of TeamSpeak audio on a connection
 *
 * @param serverConnectionHandlerID the connection handler on which to initialize the specified device
 * @param modeID utf8 encoded c-string containing the mode to open the device in. Pass an empty string to use the default mode. See @ref ts3client_getPlaybackModeList and @ref ts3client_getDefaultPlayBackMode for a list of valid modes.
 * @param playbackDevice utf8 encoded c-string containing the device name of the device to open. Pass an empty string to use the default device. See @ref ts3client_getPlaybackDeviceList and @ref ts3client_getDefaultPlaybackDevice for valid devices.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_openPlaybackDevice(uint64 serverConnectionHandlerID, const char* modeID, const char* playbackDevice);

/**
 * @brief initializes a capture device for a connection handler
 *
 * Call this function to start consuming audio from the specified device and send it to the server
 *
 * @param serverConnectionHandlerID the connection handler on which to open the capture device
 * @param modeID utf8 encoded c-string containing the mode in which to open the device. Pass an empty string to use the default mode. See @ref ts3client_getCaptureModeList and @ref ts3client_getDefaultCaptureMode for a list of valid modes.
 * @param captureDevice utf8 encoded c-string containing the device name of the device to open. Pass an empty string to use the default device. See @ref ts3client_getCaptureDeviceList and @ref ts3client_getDefaultCaptureDevice for a list or valid devices.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_openCaptureDevice(uint64 serverConnectionHandlerID, const char* modeID, const char* captureDevice);

/**
 * @brief retrieve the device name that is currently used to play audio on a server
 *
 * @param serverConnectionHandlerID the connection handler to retrieve the active playback device on
 * @param result address of a variable receiving a c string of the device name currently in use.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @param isDefault address of a variable receiving whether the device in use is the default device. Pass NULL if you don't need the information
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCurrentPlaybackDeviceName(uint64 serverConnectionHandlerID, char** result, int* isDefault);

/**
 * @brief retrieve the mode the current playback device on a server is using
 *
 * @param serverConnectionHandlerID the connection handler to retrieve the playback mode on
 * @param result address of a variable receiving a c string of the playback mode currently in use.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCurrentPlayBackMode(uint64 serverConnectionHandlerID, char** result);

/**
 * @brief retrieve the device name that is currently used to capture audio on a server
 *
 * @param serverConnectionHandlerID the connection handler to retrieve the active capture device on
 * @param result address of a variable receiving a c string of the device name currently in use.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @param isDefault address of a variable receiving whether the device in use is the default device. Pass NULL if you don't need the information
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCurrentCaptureDeviceName(uint64 serverConnectionHandlerID, char** result, int* isDefault);

/**
 * @brief retrieve the mode the current capture device on a server is using
 *
 * @param serverConnectionHandlerID the connection handler to retrieve the capture mode on
 * @param result address of a variable receiving a c string of the capture mode currently in use.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCurrentCaptureMode(uint64 serverConnectionHandlerID, char** result);

/**
 * @brief Close the playback device after all currently playing sounds are done playing
 *
 * A more user friendly way of closing a playback device. The client lib will monitor and ensure
 * that any sounds that have already started playing have completely played before closing the device.
 * New sounds are not allowed to be played after calling this function.
 * This function will return right away, regardless of whether the device has been closed already or not.
 *
 * @param serverConnectionHandlerID the connection handler on which to shut down the playback device
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_initiateGracefulPlaybackShutdown(uint64 serverConnectionHandlerID);

/**
 * @brief Immediately close the current playback device on a connection handler.
 *
 * This will instantly shut down the device. Any sounds currently playing will be interrupted.
 *
 * @param serverConnectionHandlerID the connection handler on which to shut down the playback device
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_closePlaybackDevice(uint64 serverConnectionHandlerID);

/**
 * @brief Immediately close the current capture device on a connection handler.
 *
 * This will instantly shut down the device.
 *
 * @param serverConnectionHandlerID the connection handler on which to shut down the capture device
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_closeCaptureDevice(uint64 serverConnectionHandlerID);

/**
 * @brief Activate a previously opened capture device on a server connection.
 *
 * Only one server connection can receive audio from its capture device at any given time. This
 * function will set the server connection handler that is going to receive the audio from the
 * capture device opened on that connection.
 *
 * @param serverConnectionHandlerID the connection handler on which to shut down the playback device
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_activateCaptureDevice(uint64 serverConnectionHandlerID);

/**
 * @brief Play a local wave file on the playback device of the connection handler
 *
 * @param serverConnectionHandlerID the connection handler on which to play the file. Effectively sets the playback device.
 * @param path the full path of the wave file on the local file system
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_playWaveFile(uint64 serverConnectionHandlerID, const char* path);

/**
 * @brief Play a local wave file on the playback device of the connection handler.
 *
 * This is a more advanced version of ts3client_playWaveFile as it gives you a handle which can be
 * used to stop, pause, resume or even loop the wave file.
 *
 * @param serverConnectionHandlerID the connection handler on which to play the file. Effectively sets the playback device.
 * @param path the full path of the wave file on the local file system
 * @param loop Boolean value defining whether or not to loop the wave file until the handle is paused or stopped
 * @param waveHandle address of a variable to receive the handle. Use the handle to stop, pause or resume the wave playback.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_playWaveFileHandle(uint64 serverConnectionHandlerID, const char* path, int loop, uint64* waveHandle);

/**
 * @brief Pauses or resumes playback of a wave file handle retrieved by ts3client_playWaveFileHandle.
 *
 * Audio will be stopped at whatever location it is currently at and resumed from its paused location.
 *
 * @param serverConnectionHandlerID the connection handler on which the file is playing.
 * @param waveHandle a wave handle on the specified connection handler as retrieved by ts3client_playWaveFileHandle
 * @param pause Boolean value defining whether to pause or resume the waveHandle
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_pauseWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle, int pause);

/**
 * @brief Stops playback of, closes the wave file and invalidates the handle retrieved by ts3client_playWaveFileHandle
 *
 * @param serverConnectionHandlerID the connection handler on which the file is playing.
 * @param waveHandle a wave handle on the specified connection handler as retrieved by ts3client_playWaveFileHandle
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_playWaveFileHandle
 */
EXPORTDLL unsigned int ts3client_closeWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle);

/**
 * @brief create a new software device to be used for playback and/or capture.
 *
 * This allows you to create custom devices for implementing your own audio capture or playback.
 * For capture devices you will need to regularly provide audio data via the @ref ts3client_processCustomCaptureData function.
 * For playback devices you will need to regularly aquire audio data via the @ref ts3client_acquireCustomPlaybackData function.
 *
 * @param deviceID a unique string by which you will refer to this audio device when opening devices ore removing it.
 * @param deviceDisplayName custom display string for your device. Not required to be unique, you can freely choose this.
 * @param capFrequency The frequency of the capture device. Determines the frequency the audio you're passing in to @ref ts3client_processCustomCaptureData is expected to be in when using this device.
 * @param capChannels The amount of channels the audio source on this device has. Determines the number of audio channels the data you're passing to @ref ts3client_processCustomCaptureData is expected to have when using this device.
 * @param playFrequency Determines which frequency the audio you're getting out of @ref ts3client_acquireCustomPlaybackData has when using this device.
 * @param playChannels Determines the number of audio channels of the audio you're getting out of @ref ts3client_acquireCustomPlaybackData has when using this device.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_registerCustomDevice(const char* deviceID, const char* deviceDisplayName, int capFrequency, int capChannels, int playFrequency, int playChannels);

/**
 * @brief Removes a custom audio device previously registered.
 *
 * @param deviceID the deviceID of the custom device to remove. Must be a deviceID previously passed to a @ref ts3client_registerCustomDevice call.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_unregisterCustomDevice(const char* deviceID);

/**
 * @brief Provide audio data for a registered custom device.
 *
 * When using custom devices, you're expected to call this function regularly to provide your audio data to the client lib for processing and sending it to the server.
 * The audio will be sent to the connection handler that currently has the specified custom device active (if any).
 * The client lib will read captureChannels * samples * sizeof(short) bytes of data from the buffer.
 *
 * @param deviceName the deviceID for which you're providing audio data. Must be a deviceID previously passed to a @ref ts3client_registerCustomDevice call.
 * @param buffer pointer to the beginning of the raw audio data for the device. Caller must ensure that enough data is present in the buffer (samples * channel count of the audio device).
 * @param samples the number of audio frames in the buffer
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_processCustomCaptureData(const char* deviceName, const short* buffer, int samples);

/**
 * @brief Retrieve playback data for the specified device from the client lib
 *
 * When using custom playback devices you're expected to call this function regularly.
 *
 * @param deviceName the deviceID from which to retrieve audio data. Must be a deviceID previously passed to a @ref ts3client_registerCustomDevice call.
 * @param buffer address in which to write the sound data that is pending playback. Caller must allocate sufficient memory (samples * channels of the audio device).
 * @param samples how many audio frames to retrieve.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason.
 * May return ERROR_sound_no_data meaning no sound is currently played on the device. No data has been written to the buffer.
*/
EXPORTDLL unsigned int ts3client_acquireCustomPlaybackData(const char* deviceName, short* buffer, int samples);

/**
 * @brief Route captured audio directly to the playback device rather than through the network.
 *
 * Enable or disable local test mode. Enabling will no longer send audio data to the server, instead it will
 * be routed directly to the playback device. This allows a user to receive direct feedback from their own
 * audio transmission, allowing easier adjustments to audio settings.
 *
 * @param serverConnectionHandlerID the connection handler on which to enable or disable test mode. Determines the audio devices used.
 * @param status Boolean value either enabling test mode or disabling it.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setLocalTestMode(uint64 serverConnectionHandlerID, int status);

/**
 * @brief Flags the client as recording received audio transmissions.
 *
 * This does NOT cause any recording to take place, it merely informs other clients that this client is
 * actually recording the conversation.
 *
 * @param serverConnectionHandlerID the connection handler on which to flag this client for recording.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_startVoiceRecording(uint64 serverConnectionHandlerID);

/**
 * @brief Flags the client as no longer recording audio transmissions.
 *
 * Unsets the flag set by ts3client_startVoiceRecording causing other clients to no longer mark this
 * client as recording the conversation.
 *
 * @param serverConnectionHandlerID the connection handler on which to unset the recording flag for this client.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_stopVoiceRecording(uint64 serverConnectionHandlerID);

/**
 * @brief Allow another client to whisper us.
 *
 * Adds the specified other client on the server to whisper us. Prior to this call whispers from other clients
 * are ignored and no audio data will be made available from whispers. Can be undone using @ref ts3client_removeFromAllowedWhispersFrom
 *
 * @param serverConnectionHandlerID the connection handler for the server on which the client specified by clID is located.
 * @param clID the client id of another client which we want to receive whispers from.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_setWhisperReceiveWhitelist
 * @sa ts3client_removeFromAllowedWhispersFrom
*/
EXPORTDLL unsigned int ts3client_allowWhispersFrom(uint64 serverConnectionHandlerID, anyID clID);

/**
 * @brief Removes a client from the allowed whisper list.
 *
 * Removes the specified other client on the server from the allowed whisperer list. After this call no more audio
 * is made available when receiving whispers from the specified client. The opposite of @ref ts3client_allowWhispersFrom
 *
 * @param serverConnectionHandlerID the connection handler for the server on which the client specified by clID is located.
 * @param clID the client id of another client which we do not want to receive whispers from anymore.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_setWhisperReceiveWhitelist
 * @sa ts3client_allowWhispersFrom
*/
EXPORTDLL unsigned int ts3client_removeFromAllowedWhispersFrom(uint64 serverConnectionHandlerID, anyID clID);

/**
 * @brief Retrieve the list of clients we allow to whisper us
 *
 * @since 3.0.9.0
 * @param serverConnectionHandlerID the connection handler for which to retrieve the list of clients.
 * @param result Address of an array of anyID which receives the list of clients we are allowing whispers from.
 * Memory is allocated by the client lib and caller must free the array using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_setWhisperReceiveWhitelist
*/
EXPORTDLL unsigned int ts3client_getWhisperReceiveWhitelist(uint64 serverConnectionHandlerID, anyID** result);

/**
 * @brief Check if we allow receiving whispers from a client
 *
 * @since 3.0.9.0
 * @param serverConnectionHandlerID the connection handler for the server on which the client specified by clID is located.
 * @param clientID the client id of the client to check.
 * @param result address of a variable to receive the boolean status on whether or not we allow whisper from the specified client.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_isWhisperReceiveWhitelisted(uint64 serverConnectionHandlerID, anyID clientID, int* result);

/**
 * @brief Set the list of clients we allow to whisper us
 *
 * @since 3.0.9.0
 * @param serverConnectionHandlerID the connection handler on which to set the list of clients.
 * @param clientIDs NULL terminated array of client ids that we want to allow receiving whispers from.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_getWhisperReceiveWhitelist
 * @sa ts3client_allowWhispersFrom
*/
EXPORTDLL unsigned int ts3client_setWhisperReceiveWhitelist(uint64 serverConnectionHandlerID, anyID* clientIDs);

/* 3d sound positioning */
/**
 * @brief Set position, orientation and velocity of own client in 3D space
 *
 * @param serverConnectionHandlerID the connection handler for which to set the specified 3D settings.
 * @param position 3D position of own client, pass NULL to ignore.
 * @param forward Forward orientation. Vector must be of unit length and perpendicular to the up vector. Pass NULL to ignore.
 * @param up Upward orientation. Vector must be of unit length and perpendicular to the forward vector. Pass NULL to ignore.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_systemset3DListenerAttributes(uint64 serverConnectionHandlerID, const TS3_VECTOR* position, const TS3_VECTOR* forward, const TS3_VECTOR* up);

/**
 * @brief Set the 3D position of a wave handle as retrieved by ts3client_playWaveFileHandle
 *
 * @param serverConnectionHandlerID the connection handler of the wave handle
 * @param waveHandle a valid wave Handle as retrieved by ts3client_openWaveFileHandle. Specifies the sound file for which to adjust the position
 * @param position the position the wave file should be played from
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_playWaveFileHandle
*/
EXPORTDLL unsigned int ts3client_set3DWaveAttributes(uint64 serverConnectionHandlerID, uint64 waveHandle, const TS3_VECTOR* position);

/**
 * @brief Change 3D sound attenuation and distance settings
 *
 * @param serverConnectionHandlerID the connection handler for which to adjust the settings.
 * @param distanceFactor relative distance factor in meters. Default is 1.0. Use to adjust the distance between two @ref TS3_VECTOR. Distance on x axis in meters = (a.x - b.x) * distanceFactor
 * @param rolloffScale Defines how fast sound volume will attenuate with distance. A higher value will cause sound to be toned down faster with increasing distance.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_systemset3DSettings(uint64 serverConnectionHandlerID, float distanceFactor, float rolloffScale);

/**
 * @brief Adjusts other clients position in 3D space
 *
 * @param serverConnectionHandlerID the connection handler for the server on which the client specified by clID is located.
 * @param clientID the client id of the other client we want to adjust the position of.
 * @param position the desired position in 3D space of the other client
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_channelset3DAttributes(uint64 serverConnectionHandlerID, anyID clientID, const TS3_VECTOR* position);

/*preprocessor*/
/**
 * @brief Retrieve floating point preprocessor configuration values
 *
 * @param serverConnectionHandlerID the connection handler for which to retrieve the value
 * @param ident the name of the preprocessor value to retrieve
 * @param result address of a variable to receive the specified configuration value
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getPreProcessorInfoValueFloat(uint64 serverConnectionHandlerID, const char* ident, float* result);

/**
 * @brief Retrieve preprocessor configuration values
 *
 * Preprocessor settings are bound to a capture device. You must open a capture device on the specified connection handler before
 * calling this function.
 *
 * @param serverConnectionHandlerID the connection handler for which to retrieve the configuration value
 * @param ident the name of the preprocessor configuration to retrieve
 * @param result address of a variable to receive a c string with the value of the specified preprocessor configuration.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char* ident, char** result);

/**
 * @brief Set preprocessor configuration values
 *
 * Preprocessor settings are bound to a capture device. You must open a capture device on the specified connection handler before
 * calling this function.
 *
 * @param serverConnectionHandlerID the connection handler for which to retrieve the configuration value
 * @param ident the name of the preprocessor configuration to retrieve
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char* ident, const char* value);

/**
 * @brief Indicates to the client that a key press has occurred and that it should run the typing attenuation algoritm.
 *
 * This will hint to the client lib that the typing attenuation code should be applied to the currently processed chunk
 * of audio data.
 * Effectively sets a flag in the client lib to run the code for the currently processed chunk of audio data. The client
 * will reset this flag after the current audio chunk has been completed.
 *
 * @since 3.0.9.0
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setKeyPressedDuringChunk();

/**
 * @brief Gets global client configuration values.
 *
 * ident can have the following values:
 *   input_deactivation_delay_ms: Number of milliseconds to continue transmitting after PTT key was released.
 *   input_deactivation_delay_active: Whether the above described delay is active or not.
 *
 * @since 3.0.9.0
 * @param ident the name of the global configuration option to retrieve
 * @param result address of a variable to receive the configuration value
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getGlobalConfigValueAsInt(const char* ident, int* result);

/**
 * @brief Allows changing global client configuration values.
 *
 * ident can have the following values:
 *   input_deactivation_delay_ms: Number of milliseconds to continue transmitting after PTT key was released.
 *   input_deactivation_delay_active: Whether the above described delay is active or not.
 *
 * @since 3.0.9.0
 * @param ident the name of the global configuration option to set
 * @param value the new value for the configuration option
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setGlobalConfigValue(const char* ident, const char* value);

/*encoder*/

/**
 * @brief Retrieve voice encoder information.
 *
 * Encoder options are bound to a capture device. You must open a capture device on the specified
 * connection handler prior to calling this function.
 * bitrate will return the estimated bitrate of audio without any overhead.
 * name will return the used codec name.
 * quality will return the codec quality setting, a value between 0 and 10 inclusive.
 *
 * @param serverConnectionHandlerID the connection handler to query the encoder information for.
 * @param ident the configuration value to query. Valid values are name, quality and bitrate
 * @param result address of a variable to receive an utf8 encoded c string with the value of the option queried.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getEncodeConfigValue(uint64 serverConnectionHandlerID, const char* ident, char** result);

/*playback*/

/**
 * @brief Retrieve floating point playback configuration settings
 *
 * @param serverConnectionHandlerID the connection handler to query the playback setting for.
 * @param ident the name of the configuration setting to retrieve. Valid values are volume_modifier and volume_factor_wave
 * @param result address of a variable to receive the current value of the queried setting
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const char* ident, float* result);

/**
 * @brief Set playback configuration settings
 *
 * @param serverConnectionHandlerID the connection handler to set the playback setting on.
 * @param ident the name of the configuration setting to set. Valid values are volume_modifier and volume_factor_wave
 * @param value the new value to set as an utf8 encoded c string. Appropriate conversion takes place within the client lib.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setPlaybackConfigValue(uint64 serverConnectionHandlerID, const char* ident, const char* value);

/**
 * @brief Adjust playback volume of an individual client
 *
 * Allows adjustment of single clients in addition to the global playback volume_modifier configuration option.
 * Individual client volume adjustments are temporary and only valid as long as the client is visible. Once the
 * target client leaves to an unsubscribed channel or disconnects from the server, this setting is discarded.
 * If desired, the adjustment needs to be made again after the client reconnects or becomes visible again.
 *
 * @param serverConnectionHandlerID the connection handler for the server on which the client is located
 * @param clientID the id of the client to adjust the volume for.
 * @param value the volume modifier to apply to the client.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setClientVolumeModifier(uint64 serverConnectionHandlerID, anyID clientID, float value);

/*logging*/

/**
 * @brief Log a message to the client log
 *
 * @param logMessage utf8 encoded c string of the message to log
 * @param severity the seriousness of the message logged
 * @param channel arbitrary utf8 encoded c string used to group messages. Pass empty string if unused.
 * @param logID a connection handler on which to log the message. Pass 0 if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_logMessage(const char* logMessage, enum LogLevel severity, const char* channel, uint64 logID);

/**
 * @brief When using custom logging define the severity of log messages above which to call the onUserLoggingMessageEvent for
 *
 * @param logVerbosity the verbosity above which to call the logging callback for.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/

/**
 * @brief Retrieve human readable description for an error code
 *
 * @param errorCode the error code from the @ref Ts3ErrorType enum to retrieve the description for
 * @param error address of a variable to receive a c string with the error description.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getErrorMessage(unsigned int errorCode, char** error);

/*Interacting with the server*/

/**
 * @brief initiates a connection to a TeamSpeak server.
 *
 * When using a hostname instead of an IP address, this function will block until the client lib resolved the host name.
 *
 * @param serverConnectionHandlerID the connection handler to connect on, as created by ts3client_spawnNewServerConnectionHandler
 * @param identity an identity string, as created by ts3client_createIdentity
 * @param ip the server address to connect to. Can be a hostname or an IPv4 or IPv6 address
 * @param port UDP port on which the TeamSpeak server is listening
 * @param nickname a utf8 encoded c string used to display this client to other clients on the server. Not guaranteed to be the final name.
 * @param defaultChannelArray An array describing the path to a channel to join after connect. Pass NULL when not used
 * @param defaultChannelPassword The password for the channel in defaultChannelArray. Pass empty string if unused
 * @param serverPassword server password. Pass empty string if the server does not have a password set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_startConnectionWithChannelID
*/
EXPORTDLL unsigned int ts3client_startConnection(uint64 serverConnectionHandlerID, const char* identity, const char* ip, unsigned int port, const char* nickname,
                                                 const char** defaultChannelArray, const char* defaultChannelPassword, const char* serverPassword);

/**
 * @brief initiates a connection to a TeamSpeak server.
 *
 * When using a hostname instead of an IP address, this function will block until the client lib resolved the host name.
 *
 * @param serverConnectionHandlerID the connection handler to connect on, as created by ts3client_spawnNewServerConnectionHandler
 * @param identity an identity string, as created by ts3client_createIdentity
 * @param ip the server address to connect to. Can be a hostname or an IPv4 or IPv6 address
 * @param port UDP port on which the TeamSpeak server is listening
 * @param nickname a utf8 encoded c string used to display this client to other clients on the server. Not guaranteed to be the final name.
 * @param defaultChannelId The channel id of the channel to join on connect. Pass 0 to join server default channel
 * @param defaultChannelPassword The password for the channel in defaultChannelId. Pass empty string if unused
 * @param serverPassword server password. Pass empty string if the server does not have a password set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_startConnection
*/
EXPORTDLL unsigned int ts3client_startConnectionWithChannelID(uint64 serverConnectionHandlerID, const char* identity, const char* ip, unsigned int port, const char* nickname,
                                                              uint64 defaultChannelId, const char* defaultChannelPassword, const char* serverPassword);
/**
 * @brief Disconnect from a TeamSpeak server
 *
 * @param serverConnectionHandlerID the connection handler to disconnect on
 * @param quitMessage an optional utf8 encoded message to display to other clients. Pass empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_stopConnection(uint64 serverConnectionHandlerID, const char* quitMessage);

/**
 * @brief Attempt to move one or more clients to a different channel
 *
 * The move is requested from the server. See the onServerErrorEvent callback to know whether the move was successful or not.
 *
 * @param serverConnectionHandlerID the connection handler of which the channel and client are located
 * @param clientIDArray NULL terminated array of client ids to move
 * @param newChannelID the target channel id to move the clients to
 * @param password the password for the channel. Pass an empty string if the channel has no password.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientMove(uint64 serverConnectionHandlerID, const anyID* clientIDArray, uint64 newChannelID, const char* password, const char* returnCode);

/**
 * @brief Ask the server to provide additional request only variables for a client.
 *
 * You will receive an onUpdateClientEvent callback when the data is available to you.
 *
 * @param serverConnectionHandlerID the connection handler on which the client is located
 * @param clientID the client for which to receive the client variables
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientVariables(uint64 serverConnectionHandlerID, anyID clientID, const char* returnCode);

/**
 * @brief Request client(s) to be kicked from their current channel.
 *
 * Kicking a client is essentially a glorified move to the server default channel with a message displayed to everyone.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which the client is located
 * @param clientIDArray a NULL terminated array of client Ids to kick from their current channel.
 * @param kickReason an explanatory message to display as the reason for everyone.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientKickFromChannel(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* kickReason, const char* returnCode);

/**
 * @brief Request client(s) to be kicked from the server.
 *
 * The clients will be disconnected and shown the reason. Reason is also displayed to everyone else on the server.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which the client is located
 * @param clientIDArray a NULL terminated array of client Ids to kick from their current channel.
 * @param kickReason an explanatory message to display as the reason for everyone.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientKickFromServer(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* kickReason, const char* returnCode);

/**
 * @brief Request a channel to be deleted.
 *
 * Whether or not this was successful can be determined through the associated onServerErrorEvent callback.
 *
 * @param serverConnectionHandlerID the connection handler on which the channel is located
 * @param channelID the channel id to delete
 * @param force boolean value on whether to kick clients out and delete any sub channels before deleting the channel. 1 = kick everyone, then delete sub channels and finally the requested channel; 0 = fail if there are clients in the channel or the channel has sub channels.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelDelete(uint64 serverConnectionHandlerID, uint64 channelID, int force, const char* returnCode);

/**
 * @brief Move a channel in a tree or to a different parent channel.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which the channel is located
 * @param channelID the channel id to move or change the parent of
 * @param newChannelParentID the channel id of the channel to be the new parent channel
 * @param newChannelOrder the channel id of the channel below which the channel is to be sorted
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelMove(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, uint64 newChannelOrder, const char* returnCode);

/**
 * @brief Send a private chat message to a client.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to send the message
 * @param message a utf8 encoded c string with the text to send
 * @param targetClientID the client id of the client to send the message to
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const char* message, anyID targetClientID, const char* returnCode);

/**
 * @brief Send a text message to your current channel.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to send the message
 * @param message a utf8 encoded c string with the text to send
 * @param targetChannelID the channel to send the message to. IGNORED.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestSendChannelTextMsg(uint64 serverConnectionHandlerID, const char* message, uint64 targetChannelID, const char* returnCode);

/**
 * @brief Send a text message to the server chat.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to send the message
 * @param message a utf8 encoded c string with the text to send
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestSendServerTextMsg(uint64 serverConnectionHandlerID, const char* message, const char* returnCode);

/**
 * @brief Request opening a new new-style chat room to the target user
 *
 * @param serverConnectionHandlerID the connection handler on which to open the chat
 * @param type Chat type, currently supported values. "private"
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 */
EXPORTDLL unsigned int ts3client_requestChat(uint64 serverConnectionHandlerID, const char* type, anyID targetClientID, const char* returnCode);

/**
 * @brief Request connection variables for a client (e.g. bandwidth usage, ping).
 *
 * You will receive a onConnectionInfoEvent callback once the information is available.
 *
 * @param serverConnectionHandlerID the connection handler on which the client resides
 * @param clientID which client to request the connection information for
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID, const char* returnCode);

/**
 * @brief Sets the client to which to transmit voice. Stops standard channel voice transmission.
 *
 * The client will still receive voice from their current channel, however their voice will not be transmitted to their current channel anymore.
 * If this call is successful (check onServerErrorEvent) then voice of the specified client will be transmitted to all specified channels and
 * all the specified clients.
 * Pass 0 to both target parameter arrays to restore default behavior of transmitting voice to current channel.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to set the whisper list
 * @param clientID the client to set the whisper list for. Set to 0 or your own client ID to set your own whisper list.
 * @param targetChannelIDArray a zero terminated array of channel ids to transmit voice to.
 * @param targetClientIDArray a zero terminated array of client ids to transmit voice to.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientSetWhisperList(uint64 serverConnectionHandlerID, anyID clientID, const uint64* targetChannelIDArray, const anyID* targetClientIDArray, const char* returnCode);

/**
 * @brief Request live updates to specific channels, being able to see clients in the channel.
 *
 * If you intend to subscribe to all channels on the server, use ts3client_requestChannelSubscribeAll function instead.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to subscribe to the specified channels
 * @param channelIDArray a zero terminated array of channel ids to subscribe to
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelSubscribe(uint64 serverConnectionHandlerID, const uint64* channelIDArray, const char* returnCode);

/**
 * @brief Request live updates from all channels, being able to see clients in the channels.
 *
 * If you only want to subscribe to a specific subset of channels, use ts3client_requestChannelSubscribe funtion instead.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to subscribe to all channels
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelSubscribeAll(uint64 serverConnectionHandlerID, const char* returnCode);

/**
 * @brief Remove subscription from channels. No longer receiving updates to clients in the channels.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to unsubscribe from the specified channels
 * @param channelIDArray a zero terminated array of channel ids to unsubscribe from
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribe(uint64 serverConnectionHandlerID, const uint64* channelIDArray, const char* returnCode);

/**
 * @brief Remove subscription from all channels. No longer receiving updates to clients outside of own channel.
 *
 * The current channel will always be subscribed and you will always receive updates about clients in the current channel.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to unsubscribe from all channels
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribeAll(uint64 serverConnectionHandlerID, const char* returnCode);

/**
 * @brief retrieve the channel description of the specified channel.
 *
 * After calling this function you will receive an onUpdateChannelEvent callback at which point the description is available to
 * be queried using ts3client_getChannelVariableAsString.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which the channel is located
 * @param channelID the id of the channel to retrieve the description for
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestChannelDescription(uint64 serverConnectionHandlerID, uint64 channelID, const char* returnCode);

/**
 * @brief Mute clients locally, the server will not be sending audio data for the specified clients anymore.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to mute the clients
 * @param clientIDArray a zero terminated array of client ids to mute
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_requestUnmuteClients
*/
EXPORTDLL unsigned int ts3client_requestMuteClients(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);

/**
 * @brief Unmute clients locally. Server will start sending audio packets for the specified clients again.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to unmute the clients
 * @param clientIDArray a zero terminated array of client ids to unmute
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 * @sa ts3client_requestMuteClients
*/
EXPORTDLL unsigned int ts3client_requestUnmuteClients(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);

/**
 * @brief retrieve the current client ids of all clients connected using the specified unique identifier
 *
 * You will receive a onClientIDsEvent callback for every client connected with the identity specified by the clientUniqueIdentifier.
 * Once all client ids for the specified identity have been indicated, you will receive a onClientIDsFinishedEvent callback.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID the connection handler on which to retrieve the client ids for
 * @param clientUniqueIdentifier a c string with a unique identifier to retreive the client ids for. This is the public part of the identity
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestClientIDs(uint64 serverConnectionHandlerID, const char* clientUniqueIdentifier, const char* returnCode);

/*provisioning server calls*/
EXPORTDLL unsigned int ts3client_requestSlotsFromProvisioningServer(const char* ip, unsigned short port, const char* serverPassword, unsigned short slots, const char* identity, const char* region, uint64* requestHandle);
EXPORTDLL unsigned int ts3client_cancelRequestSlotsFromProvisioningServer(uint64 requestHandle);
EXPORTDLL unsigned int ts3client_startConnectionWithProvisioningKey(uint64 serverConnectionHandlerID, const char* identity, const char* nickname, const char* connectionKey, const char* clientMetaData);

/*retrieve information ClientLib has stored*/

/*general info*/

/**
 * @brief get your own client id on a server
 *
 * @param serverConnectionHandlerID the connection handler on which to retrieve your own client id
 * @param result address of a variable to receive your client id on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientID(uint64 serverConnectionHandlerID, anyID* result);

/*client connection info*/

/**
 * @brief check the current status of the connection
 *
 * @param serverConnectionHandlerID the connection handler for which to receive the connection status
 * @param result address of a variable to receive the current connect status. One of the values from the ConnectStatus enum
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getConnectionStatus(uint64 serverConnectionHandlerID, int* result);

/**
 * @brief Get value for connection based variable of a client as unsigned 64 bit integer
 *
 * Not all variables are available as unsigned 64 bit integer. Some are only available as string or double.
 *
 * @param serverConnectionHandlerID the connection handler on which to get the value
 * @param clientID the client for which to retrieve the value
 * @param flag specifies which value to receive. One of the values from the ConnectionProperties enum
 * @param result address of a variable to receive the variable on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64* result);

/**
 * @brief Get value for connection based variable of a client as double
 *
 * Not all variables are available as double. Some are only available as string or unsigned 64 bit integers.
 *
 * @param serverConnectionHandlerID the connection handler on which to get the value
 * @param clientID the client for which to retrieve the value
 * @param flag specifies which value to receive. One of the values from the ConnectionProperties enum
 * @param result address of a variable to receive the variable on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getConnectionVariableAsDouble(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, double* result);

/**
 * @brief Get value for connection based variable of a client as string
 *
 * Not all variables are available as string. Some are only available as unsigned 64 bit integer or double.
 *
 * @param serverConnectionHandlerID the connection handler on which to get the value
 * @param clientID the client for which to retrieve the value
 * @param flag specifies which value to receive. One of the values from the ConnectionProperties enum
 * @param result address of a variable to receive the variable on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getConnectionVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char** result);

/**
 * @brief TODO
 *
 * @param serverConnectionHandlerID connection handler to clean up on
 * @param clientID the client to clean up
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_cleanUpConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID);

/*server connection info*/

/**
 * @brief Make server connection variables available for retrieval
 *
 * @param serverConnectionHandlerID the connection handler for which to make connection variables available
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestServerConnectionInfo(uint64 serverConnectionHandlerID, const char* returnCode);

/**
 * @brief Retrieve value of a server connection variable as unsigned 64 bit integer
 *
 * Not all variables are available as unsigned 64 bit integer. Some are only available float.
 *
 * @param serverConnectionHandlerID the connection handler on which to retrieve the value
 * @param flag specifies which variable to receive. One of the values from the ConnectionProperties enum
 * @param result address of a variable to receive the value on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, size_t flag, uint64* result);

/**
 * @brief Retrieve value of a server connection variable as float
 *
 * Not all variables are available as float. Some are only available as unsigned 64 bit integer.
 *
 * @param serverConnectionHandlerID the connection handler on which to retrieve the value
 * @param flag specifies which variable to receive. One of the values from the ConnectionProperties enum
 * @param result address of a variable to receive the value on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerConnectionVariableAsFloat(uint64 serverConnectionHandlerID, size_t flag, float* result);

/*client info*/

/**
 * @brief Retrieve value of a variable of your own client as an integer
 *
 * Not all variables are available as integer. Some are only available as string.
 * NOTE: Not all variables are available using this function, some are only available using ts3client_getClientVariableAsInt
 *
 * @param serverConnectionHandlerID connection handler on which to retrieve information
 * @param flag specifies which variable to receive. One of the values from the ClientProperties enum
 * @param result address of a variable to receive the value on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int* result);

/**
 * @brief Retrieve value of a variable of your own client as string
 *
 * Not all variables are available as integer. Some are only available as integer.
 * NOTE: Not all variables are available using this function, some are only available using ts3client_getClientVariableAsString
 *
 * @param serverConnectionHandlerID connection handler on which to retrieve information
 * @param flag specifies which variable to receive. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param result address of a variable to receive the value on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char** result);

/**
 * @brief Change the value of an integer variable on your own client
 *
 * After having changed all variables desired, call ts3client_flushClientSelfUpdates to publish the changes to the server
 * Not all variables can be changed, many are read only.
 *
 * @param serverConnectionHandlerID the connection handler on which to set the value
 * @param flag specifies which variable to change. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int value);

/**
 * @brief Change the value of a string variable on your own client
 *
 * After having changed all variables desired, call ts3client_flushClientSelfUpdates to publish the changes to the server
 * Not all variables can be changed, many are read only.
 *
 * @param serverConnectionHandlerID the connection handler on which to set the value
 * @param flag specifies which variable to change. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, const char* value);

/**
 * @brief Send changes to the local client to the server
 *
 * Publish changes previously set using @ref ts3client_setClientSelfVariableAsInt and @ref ts3client_setClientSelfVariableAsString on the
 * connection handler.
 *
 * @param serverConnectionHandlerID connection handler on which to publish changes
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_flushClientSelfUpdates(uint64 serverConnectionHandlerID, const char* returnCode);

/**
 * @brief Retrieve the value of a variable from a client as integer
 *
 * Not all variables are available as integer. Some are only available as string or unsigned 64bit integer.
 *
 * @param serverConnectionHandlerID connection handler on which the client is located
 * @param clientID for which client to retrieve the value
 * @param flag specifies which variable to receive. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param result address of a variable to receive the value on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientVariableAsInt(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, int* result);

/**
 * @brief Retrieve the value of a variable from a client as unsigned 64bit integer
 *
 * Not all variables are available as integer. Some are only available as string or integer.
 *
 * @param serverConnectionHandlerID connection handler on which the client is located
 * @param clientID for which client to retrieve the value
 * @param flag specifies which variable to receive. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param result address of a variable to receive the value on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64* result);

/**
 * @brief Retrieve the value of a variable from a client as string
 *
 * Not all variables are available as integer. Some are only available as integer or unsigned 64bit integer.
 *
 * @param serverConnectionHandlerID connection handler on which the client is located
 * @param clientID for which client to retrieve the value
 * @param flag specifies which variable to receive. One of the values from the ClientProperties or ClientPropertiesRare enums
 * @param result address of a variable to receive the value on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char** result);

/**
 * @brief Get a list of all clients in subscribed channels on the server
 *
 * @param serverConnectionHandlerID connection handler on which to retrieve the client list
 * @param result address of a variable to receive a null terminated array of client ids like {10, 30, ..., 0}
 * Memory is allocated by the client lib and caller must free the array using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getClientList(uint64 serverConnectionHandlerID, anyID** result);

/**
 * @brief Get id of the current channel the specified client is in
 *
 * @param serverConnectionHandlerID connection handler on which the client is located
 * @param clientID the client to receive the current channel for
 * @param result address of a variable to receive the channel id of the specified client
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelOfClient(uint64 serverConnectionHandlerID, anyID clientID, uint64* result);

/*channel info*/

/**
 * @brief Retrieve the value of a channel property as integer
 *
 * Not all properties are available as integer. Some are only available as string or unsigned 64 bit integer.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID the channel of which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the ChannelProperties or ChannelPropertiesRare enum
 * @param result address of a variable to receive the result on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int* result);

/**
 * @brief Retrieve the value of a channel property as unsigned 64 bit integer
 *
 * Not all properties are available as string. Some are only available as integer or string.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID the channel of which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the ChannelProperties or ChannelPropertiesRare enum
 * @param result address of a variable to receive the result on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64* result);

/**
 * @brief Retrieve the value of a channel property as string
 *
 * Not all properties are available as string. Some are only available as integer or unsigned 64 bit integer.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID the channel of which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the ChannelProperties or ChannelPropertiesRare enum
 * @param result address of a variable to receive the result on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, char** result);

/**
 * @brief Get the channel id for the given channel path
 *
 * @param serverConnectionHandlerID connection handler on which to find the channel
 * @param channelNameArray zero terminated array of c strings describing the channel path. Like {"Main channel", "sub channel", null}
 * @param result address of a variable to receive the channel id on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelIDFromChannelNames(uint64 serverConnectionHandlerID, char** channelNameArray, uint64* result);

/**
 * @brief set a new value for an integer channel property
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to set the property for
 * @param flag specifies which property to set. One of the values from the ChannelProperties or ChannelPropertiesRare enums
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int value);

/**
 * @brief set a new value for an unsigned 64 bit channel property
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to set the property for
 * @param flag specifies which property to set. One of the values from the ChannelProperties or ChannelPropertiesRare enums
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64 value);

/**
 * @brief set a new value for a string channel property
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to set the property for
 * @param flag specifies which property to set. One of the values from the ChannelProperties or ChannelPropertiesRare enums
 * @param value the new value to set
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, const char* value);

/**
 * @brief Inform server of changes to channel properties
 *
 * After all desired changes have been done using @ref ts3client_setChannelVariableAsInt, @ref ts3client_setChannelVariableAsUInt64
 * or @ref ts3client_setChannelVariableAsString call this function to send the changes to the server and publish them to other clients.
 * Prior to calling this function the channel property changes will not have any effect.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to publish updates for
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_flushChannelUpdates(uint64 serverConnectionHandlerID, uint64 channelID, const char* returnCode);

/**
 * @brief Create the channel on the server
 *
 * After setting all the desired properties on the channel, call this function to actually create the channel on the server
 *
 * @param serverConnectionHandlerID connection handler on which to create the channel
 * @param channelParentID id of the channel this channel should be a sub channel of. Pass 0 to create a root channel.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_flushChannelCreation(uint64 serverConnectionHandlerID, uint64 channelParentID, const char* returnCode);

/**
 * @brief Get a list of all channels currently on the server
 *
 * @param serverConnectionHandlerID connection handler on which to retrieve the channels
 * @param result address of a variable to receive a zero terminated array of channel ids, like {1, 4023, 49, 8534, ..., 0}
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelList(uint64 serverConnectionHandlerID, uint64** result);

/**
 * @brief Get a list of all clients in the specified channel
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID the channel of which to retrieve the current clients
 * @param result address of a variable to receive a zero terminated array of client ids, like {2, 50, 4, ..., 0}
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID, anyID** result);

/**
 * @brief get the id of the parent channel of the specified channel.
 *
 * If the channel specified by channelID is a root channel, the result will be 0.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to retrieve the parent of
 * @param result address of a variable to receive the parent channel id.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getParentChannelOfChannel(uint64 serverConnectionHandlerID, uint64 channelID, uint64* result);

/**
 * @brief get time in seconds since last client left the specified channel
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located
 * @param channelID id of the channel to get the
 * @param result address of a variable to receive the result on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getChannelEmptySecs(uint64 serverConnectionHandlerID, uint64 channelID, int* result);

/*server info*/

/**
 * @brief get a list of all connection handlers
 *
 * @param result address of a variable to receive a zero terminated array of connection handlers, like {1, 5, ..., 0}
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerList(uint64** result);

/**
 * @brief get the value of an integer server property.
 *
 * Not all properties are available as integer. Some are only available as string or unsigned 64 bit integer.
 *
 * @param serverConnectionHandlerID specifies the server on which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the VirtualServerProperties or VirtualServerPropertiesRare enums
 * @param result address of a variable to receive the property value on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int* result);

/**
 * @brief get the value of an unsigned 64 bit integer server property.
 *
 * Not all properties are available as unsigned 64 bit integer. Some are only available as string or integer.
 *
 * @param serverConnectionHandlerID specifies the server on which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the VirtualServerProperties or VirtualServerPropertiesRare enums
 * @param result address of a variable to receive the property value on success.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerVariableAsUInt64(uint64 serverConnectionHandlerID, size_t flag, uint64* result);

/**
 * @brief get the value of a string server property.
 *
 * Not all properties are available as string. Some are only available as integer or unsigned 64 bit integer
 *
 * @param serverConnectionHandlerID specifies the server on which to retrieve the property
 * @param flag specifies which property to retrieve. One of the values from the VirtualServerProperties or VirtualServerPropertiesRare enums
 * @param result address of a variable to receive the property value on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char** result);

/**
 * @brief Make request only server variables available locally.
 *
 * You will receive an onServerUpdateEvent once the request only properties are available.
 * Prior to the callback being called the variables are not available, and querying them will yield undefined results.
 *
 * @param serverConnectionHandlerID connection handler on which to request the variables
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestServerVariables(uint64 serverConnectionHandlerID, const char* returnCode);

/*filetransfer management*/

/**
 * @brief get the local file name for a file transfer
 *
 * @param transferID identifies the file transfer to query
 * @param result address of a variable to receive an utf8 encoded c string on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferFileName(anyID transferID, char** result);

/**
 * @brief get the local path of a file transfer
 *
 * @param transferID identifies the file transfer to query
 * @param result address of a variable to receive an utf8 encoded c string on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferFilePath(anyID transferID, char** result);

/**
 * @brief get the server path of the file transfer
 *
 * @param transferID identifies which file transfer to query
 * @param result address of a variable to receive an utf8 encoded c string on success.
 * Memory is allocated by the client lib and must be freed by caller using @ref ts3client_freeMemory
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferFileRemotePath(anyID transferID, char** result);

/**
 * @brief get the total size in bytes of a file transfer.
 *
 * @param transferID specifies which file transfer to query
 * @param result address of a variable to receive the file size
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferFileSize(anyID transferID, uint64* result);

/**
 * @brief get the amount of bytes already transferred.
 *
 * 0 <= result <= ts3client_getTransferFileSize for the same transferID.
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the result on success
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferFileSizeDone(anyID transferID, uint64* result);

/**
 * @brief determine if the file transfer is an upload or download
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the result on success. 1 = upload, 0 = download
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_isTransferSender(anyID transferID, int* result);

/**
 * @brief determine the current status of the transfer in question
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the status on success. One of the values from the FileTransferState enum.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferStatus(anyID transferID, int* result);

/**
 * @brief get the current approximate speed (in bytes/sec) of a file transfer
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the transfer speed in bytes per second, averaged across the past 5 seconds.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getCurrentTransferSpeed(anyID transferID, float* result);

/**
 * @brief get the average transfer speed (in bytes/sec) of a file transfer since it started
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the approximate speed in bytes per second, averaged across its lifetime.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getAverageTransferSpeed(anyID transferID, float* result);

/**
 * @brief get the time (in seconds) a file transfer has been active
 *
 * @param transferID specifies the file transfer to query
 * @param result address of a variable to receive the time in seconds the transfer was active.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferRunTime(anyID transferID, uint64* result);

/*Interacting with the server - file transfers*/

/**
 * @brief Initiate a file upload to the server.
 *
 * @param serverConnectionHandlerID connection handler to which to upload a file
 * @param channelID channel to which to upload the file
 * @param channelPW password of the channel specified in channelID. Pass an empty string if the channel does not have a password.
 * @param file the name of file to upload on the local file system.
 * @param overwrite boolean flag, whether to overwrite the file on the server. If 0 the transfer will fail if the file already exists on the server.
 * @param resume boolean flag, set to 1 to resume a previously aborted or halted transfer. If 1 will append to the file on the server.
 * @param sourceDirectory the absolute path in which the file resides on the local file system.
 * @param result address of a variable in which to store the transferID on success.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_sendFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, int overwrite, int resume, const char* sourceDirectory, anyID* result, const char* returnCode);

/**
 * @brief Initiate a file download from the server.
 *
 * @param serverConnectionHandlerID connection handler from which to download the file
 * @param channelID channel in which the file to download is located
 * @param channelPW password of the channel specified in channelID. Pass an empty string if the channel does not have a password.
 * @param file the name of the file on the server file system. See ts3client_getFileList to receive a list of files.
 * @param overwrite boolean flag, whether to overwrite the local file if it already exists. If set to 0 transfer will fail if local file already exists unless resume is 1.
 * Mutually exclusive to resume.
 * @param resume boolean flag, whether to append to the local file. If set to 1 the contents of the download will be appended to the local file. Mutually exclusive with overwrite.
 * @param destinationDirectory absolute path to the directory in which to store the file.
 * @param result address of a variable to receive the transfer id, used to identity this request in callbacks and other calls regarding the status of this transfer
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, int overwrite, int resume, const char* destinationDirectory, anyID* result, const char* returnCode);

/**
 * @brief Cancel a file transfer
 *
 * @param serverConnectionHandlerID connection handler on which the file transfer is happening
 * @param transferID specifies the file transfer to cancel
 * @param deleteUnfinishedFile boolean flag, whether to delete the partially transmitted file from the file system.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_haltTransfer(uint64 serverConnectionHandlerID, anyID transferID, int deleteUnfinishedFile, const char* returnCode);

/**
 * @brief retrieve a list of files in a directory.
 *
 * This function is NOT recursive. Only directories and files in the directory specified by path will be listed.
 * You will receive a onFileListEvent callback for every file or directory after this function was successful.
 * Once all files and directories were sent you will receive a onFileListFinishedEvent callback.
 *
 * @param serverConnectionHandlerID the connection handler on which to request files
 * @param channelID the channel from which to list the files
 * @param channelPW the password of the specified channel. Pass an empty string if the channel has no password.
 * @param path the path in the specified channel from which to list the files. Pass "/" to list the files in the root channel.
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestFileList(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* path, const char* returnCode);

/**
 * @brief retrieve information about a specific file.
 *
 * You will receive an onFileInfoEvent callback after this function was successful.
 *
 * @param serverConnectionHandlerID connection handler on which to request the file information.
 * @param channelID the channel in which the file is located
 * @param channelPW the password of the specified channel. Pass an empty string if the channel has no password.
 * @param file absolute path to the file to query information of. Must begin with "/".
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestFileInfo(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, const char* returnCode);

/**
 * @brief delete one or more files from a channel.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID connection handler on which to delete the file
 * @param channelID the channel in which the file is located
 * @param channelPW the password of the specified channel. Pass an empty string if the channel has no password.
 * @param file a zero terminated array of absolute paths to the files to delete. Each path must begin with "/". Like {"/file.txt", "/dir/subdir/test.txt", ..., 0}
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestDeleteFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char** file, const char* returnCode);

/**
 * @brief create a directory in a channel for file organization
 *
 * Note: This will NOT recursively create directories. If you need recursive creation call this function again after the intended parent directory has been created.
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID connection handler on which the channel is located.
 * @param channelID the channel in which the file is located
 * @param channelPW the password of the specified channel. Pass an empty string if the channel has no password.
 * @param directoryPath absolute path of the directory to create. Must start with "/" e.g. "/existing/newDirName"
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestCreateDirectory(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* directoryPath, const char* returnCode);

/**
 * @brief move or rename a file on the server.
 *
 * You will receive an onServerErrorEvent with the passed returnCode indicating whether or not the operation was successful.
 *
 * @param serverConnectionHandlerID connection handler on which to move/rename the file
 * @param fromChannelID channel the file is currently located in
 * @param fromChannelPW password of the specified channel. Pass an empty string if the channel has no password.
 * @param toChannelID channel id to which to move the file to. Pass the same value as fromChannelID to keep the file in the same channel.
 * @param toChannelPW password of the target channel. Pass an empty string if the channel has no password.
 * @param oldFile current absolute path of the file in the channel. Must start with "/".
 * @param newFile new absolute path of the file in the target channel. Must start with "/". e.g. "/subdirectory/filename.txt"
 * @param returnCode a c string to identify this request in callbacks. Pass an empty string if unused.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_requestRenameFile(uint64 serverConnectionHandlerID, uint64 fromChannelID, const char* fromChannelPW, uint64 toChannelID, const char* toChannelPW, const char* oldFile, const char* newFile, const char* returnCode);

/**
 * @brief get the configured maximum upload speed of the server instance.
 *
 * The limit is temporary and valid only until ts3client_destroyClientLib is called.
 *
 * @param limit address of a variable to receive the limit in bytes per second.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getInstanceSpeedLimitUp(uint64 *limit);

/**
 * @brief get the configured maximum download speed of the server instance.
 *
 * The limit is temporary and valid only until ts3client_destroyClientLib is called.
 *
 * @param limit address of a variable to receive the limit in bytes per second.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getInstanceSpeedLimitDown(uint64 *limit);

/**
 * @brief get the configured maximum upload speed for the virtual server.
 *
 * Upload speeds on this server will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param serverConnectionHandlerID connection handler to query the value on.
 * @param limit address of a variable to receive the limit in bytes per second.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerSpeedLimitUp(uint64 serverConnectionHandlerID, uint64* limit);

/**
 * @brief get the configured maximum download speed for the virtual server.
 *
 * Download speeds on this server will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param serverConnectionHandlerID connection handler to query the value on.
 * @param limit address of a variable to receive the limit in bytes per second.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerSpeedLimitDown(uint64 serverConnectionHandlerID, uint64* limit);

/**
 * @brief get the speed limit for a specific file transfer.
 *
 * @param transferID specifies which transfer to query.
 * @param limit address of a variable to receive the transfer limit in bytes per second.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_getTransferSpeedLimit(anyID transferID, uint64* limit);

/**
 * @brief set the instance wide upload speed limit for file transfer.
 *
 * All concurrent file transfers combined will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param newLimit maximum upload speed in bytes per second. Must be >= 5120.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setInstanceSpeedLimitUp(uint64 newLimit);

/**
 * @brief set the instance wide download speed limit for file transfer.
 *
 * All concurrent file transfers combined will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param newLimit maximum download speed in bytes per second. Must be >= 5120.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setInstanceSpeedLimitDown(uint64 newLimit);

/**
 * @brief set the virtual server upload speed limit for file transfer.
 *
 * All concurrent file transfers combined will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param serverConnectionHandlerID connection handler on which to set the limit.
 * @param newLimit maximum upload speed in bytes per second. Must be >= 5120.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setServerConnectionHandlerSpeedLimitUp(uint64 serverConnectionHandlerID, uint64 newLimit);

/**
 * @brief set the virtual server download speed limit for file transfer.
 *
 * All concurrent file transfers combined will not exceed min(instance limit, virtual server limit) bytes per second.
 * The limit is temporary and valid only for the lifetime of the connection handler.
 *
 * @param serverConnectionHandlerID connection handler on which to set the limit.
 * @param newLimit maximum download speed in bytes per second. Must be >= 5120.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setServerConnectionHandlerSpeedLimitDown(uint64 serverConnectionHandlerID, uint64 newLimit);

/**
 * @brief set the transfer limit for an individual file transfer.
 *
 * The maximum transfer speed will be min(instance limit, virtual server limit, transfer limit).
 * Whether the limit is upload or download depends on what kind of transfer the specified transfer is.
 *
 * @param transferID the transfer to set the limit for
 * @param newLimit the new maximum speed in bytes per second. Must be >= 5120.
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
*/
EXPORTDLL unsigned int ts3client_setTransferSpeedLimit(anyID transferID, uint64 newLimit);

/**
 * @brief Request a login token for the chat server associated with the specified virtual server
 * 
 * @param serverConnectionHandlerID connection handler on which to request the chat login token
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 */
EXPORTDLL unsigned int ts3client_getChatLoginToken(uint64 serverConnectionHandlerID);

/**
 * @brief Request an authentication token from the specified virtual server
 *
 * @param serverConnectionHandlerID connection handler on which to request the authentication token
 * @return An Error code from the @ref Ts3ErrorType enum indicating either success or the failure reason
 */
EXPORTDLL unsigned int ts3client_getAuthenticationToken(uint64 serverConnectionHandlerID);

#ifdef __cplusplus
}
#endif

#endif
