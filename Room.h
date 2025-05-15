#ifndef __ROOM_H__
#define __ROOM_H__

#include "../OuterFactoryImp.h"
#include "./QSTableMng/QSTableMng.h"
#include "./KOTableMng/KOTableMng.h"
#include "./SNGTableMng/SNGTableMng.h"
#include "./PRTableMng/PRTableMng.h"
#include "./AITableMng/AITableMng.h"
#include "./SHTableMng/SHTableMng.h"
#include "./DDZTableMng/DDZTableMng.h"
#include "./MTTableMng/MTTableMng.h"

/**
 *
*/
class CRoom : public TC_HandleBase
{
public:
    /**
     *
    */
    CRoom();

    /**
     *
    */
    ~CRoom();

public:
    /**
     * 初始化
     */
    int initialize();

    /**
     *  加载游戏逻辑so
     */
    int initGameSo(TableConfInfo &stTableConfInfo);

    /**
     *  初始化桌子管理
     */
    int initTableMng(TableConfInfo &stTableConfInfo);


public:
    /**
     * 获取管理对象列表
     */
    std::map<string, CTableMng *> &getRID2TMngMap()
    {
        return m_mapRID2TMng;
    }

    /**
     * 插入管理对象列表
     */
    int insertRID2TMngMap(std::string sRoomID, CTableMng *pTableMng);

    /**
     * 房间是否存在
     */
    bool isExistRID4Map(std::string sRoomID);

    /**
     * 查找房间管理对象
     */
    CTableMng *findTMngByRID(std::string sRoomID);

    /**
     * 房间状态改变
     */
    int roomStatusChange(std::string sRoomID);

    /**
     * 查看房间信息
     */
    void showRInfo(const string &params, string &result);

private:
    // @param roomid, TableMng
    std::map<string, CTableMng *>  m_mapRID2TMng;
};

//
typedef TC_Singleton<CRoom, CreateStatic, DefaultLifetime> CRoomSingleton;

#endif



