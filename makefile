
#-----------------------------------------------------------------------

APP            := XGame
TARGET         := RoomServer
STRIP_FLAG     := N
TARS2CPP_FLAG  := 
CFLAGS         += -lm -ldl -Wunused-variable
CXXFLAGS       += -lm -ldl -Wunused-variable

INCLUDE   +=  -I. -I/usr/local/cpp_modules/protobuf/include
LIB       += /usr/local/cpp_modules/protobuf/lib/libprotobuf.a

INCLUDE   += -I/usr/local/cpp_modules/wbl/include
LIB       += -L/usr/local/cpp_modules/wbl/lib -lwbl -ldl

INCLUDE   += -I./XTimer -I./AsyncOperate -I./AsyncOperate/MatchServer\
		 -I./AsyncOperate/GMServer -I./AsyncOperate/HallServer -I./AsyncOperate/AIServer\
	         -I./CommTableMng -I./QSTableMng -I./KOTableMng -I./RGTableMng -I./SHTableMng\
	         -I./AITableMng -I./SNGTableMng -I./RGTableMng -I./PRTableMng -I./AIMng -I./DDZTableMng \
	         -I./MJTableMng -I./MTTableMng

LOCAL_SRC += AIMng/AiMng.cpp

INCLUDE   += -I./AIMng/MonteCarlo -I./AIMng/ThirdParty
LOCAL_SRC += AIMng/MonteCarlo/cards.cpp AIMng/MonteCarlo/samples.cpp AIMng/MonteCarlo/simulator.cpp \
 			 AIMng/MonteCarlo/tables.cpp AIMng/MonteCarlo/tools.cpp

LOCAL_SRC += AIMng/ThirdParty/ThirdPartyManager.cpp AIMng/ThirdParty/AsyncEpoller.cpp \
			 AIMng/ThirdParty/AsyncSocket.cpp AIMng/ThirdParty/ThirdLog.cpp \
			 AIMng/ThirdParty/NetMsg.cpp AIMng/ThirdParty/RawBuffer.cpp AIMng/ThirdParty/TcpClient.cpp

LOCAL_SRC += QSTableMng/QSTableMng.cpp KOTableMng/KOTableMng.cpp SHTableMng/SHTableMng.cpp\
			 SNGTableMng/SNGTableMng.cpp PRTableMng/PRTableMng.cpp AITableMng/AITableMng.cpp\
			 CommTableMng/RTableMng.cpp CommTableMng/GTableMng.cpp CommTableMng/MTableMng.cpp \
			 DDZTableMng/DDZTableMng.cpp MJTableMng/MJTableMng.cpp MTTableMng/MTTableMng.cpp \
			 XTimer/xtimer.cpp XTimer/xtime_userface.cpp 

LOCAL_SRC += AsyncOperate/MatchServer/AsyncAdditionalConsumeCallback.cpp \
			 AsyncOperate/MatchServer/AsyncRepurchaseCheckCallback.cpp \
			 AsyncOperate/MatchServer/AsyncAdditionalCheckCallback.cpp \
			 AsyncOperate/MatchServer/AsyncUpdateRiseCallback.cpp \
			 AsyncOperate/MatchServer/AsyncRepurchaseConsumeCallback.cpp \
			 AsyncOperate/MatchServer/AsyncQuitCallback.cpp \
			 AsyncOperate/MatchServer/AsyncSignUpCallback.cpp \
			 AsyncOperate/MatchServer/AsyncRewardCallback.cpp \
			 AsyncOperate/MatchServer/AsyncUpdateUserSegmentInfoCallback.cpp \
			 AsyncOperate/MatchServer/AsyncMatchCallback.cpp 

LOCAL_SRC += AsyncOperate/HallServer/AsyncUserInfoCallback.cpp \
			 AsyncOperate/HallServer/AsyncUserSegmentInfoCallback.cpp \
			 AsyncOperate/HallServer/AsyncLoginRoomCallback.cpp
			
LOCAL_SRC += AsyncOperate/GMServer/AsyncGetCardsCallback.cpp

LOCAL_SRC += AsyncOperate/AIServer/AsyncDoRobotDecide.cpp \
			 AsyncOperate/AIServer/AsyncAIDecide.cpp \
			 AsyncOperate/AIServer/AsyncAIMsgTransmit.cpp

#-----------------------------------------------------------------------
include /home/tarsproto/XGame/Comm/Comm.mk
include /home/tarsproto/XGame/util/util.mk
include /home/tarsproto/XGame/protocols/protocols.mk
include /home/tarsproto/XGame/RouterServer/RouterServer.mk
include /home/tarsproto/XGame/HallServer/HallServer.mk
include /home/tarsproto/XGame/ConfigServer/ConfigServer.mk
include /home/tarsproto/XGame/GlobalServer/GlobalServer.mk
include /home/tarsproto/XGame/PushServer/PushServer.mk
include /home/tarsproto/XGame/GameRecordServer/GameRecordServer.mk
include /home/tarsproto/XGame/MatchServer/MatchServer.mk
include /home/tarsproto/XGame/AIServer/AIServer.mk
include /home/tarsproto/XGame/GMServer/GMServer.mk
include /home/tarsproto/XGame/ActivityServer/ActivityServer.mk
include /home/tarsproto/XGame/Log2DBServer/Log2DBServer.mk
include /home/tarsproto/XGame/SocialServer/SocialServer.mk
include /home/tarsproto/XGame/RoomServer/RoomServer.mk
include /usr/local/tars/cpp/makefile/makefile.tars
#-----------------------------------------------------------------------

xgame:
	sshpass -p 'Depu+2020' scp ./RoomServer  root@10.10.10.23:/usr/local/app/tars/tarsnode/data/SoHa.RoomServer/bin/

qsgame:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/QSGame.RoomServer/bin/

qsgamea:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/QSGameA.RoomServer/bin

qsgamei:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/QSGameI.RoomServer/bin

kogame:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/KOGame.RoomServer/bin/

kogamea:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/KOGameA.RoomServer/bin/

kogameb:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/KOGameB.RoomServer/bin/

kogamec:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/KOGameC.RoomServer/bin/

kogamed:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/KOGameD.RoomServer/bin/

