#ifndef HAN_DEF_H
#define HAN_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

// �����
typedef enum HAN_lib_errno_enum HAN_errno_t; // C++ ���������ܻ� enum class �ı�����������Ըĳ� typedef int HAN_errno_t;
enum HAN_lib_errno_enum {
    RET_OK,                 // ����ִ������
    NOT_ENOUGH_MEMORY,      // �ڴ治��
    /* FILE */
    OPEN_FILE_ERR,          // ���ļ��������Ҫ֪�����������ִ���Ҫϵͳ�ṩ�ĺ���ȷ��
    BLANK_LINE_EOF,         // ʹ�� GetStrLine ���ļ���ȡһ������ʱ�������ȡ�����ļ�β�Ŀ��У��᷵�ظ�ֵ�����һ�����ַ���������ļ�β���ǿ��У���������ȡ������ RET_OK
    /* HAN_data_structure */
    INDEX_OUT_OF_RANGE,     // ��ȡ�б�����ʱ�ṩ������ֵ�����б���
    END_OF_LIST,            // ʹ�� ForList ���������б�ʱ����������ʱ�᷵�ظ�ֵ
    KEY_NOT_EXIST,          // ��ȡ�ֵ�����ʱ�ṩ�ļ�ֵ������
    END_OF_DICT,            // ʹ�� ForDict ���������ֵ�ʱ����������ʱ�᷵�ظ�ֵ
    TITLE_EXIST,            // �����Ѵ���
    TITLE_NOT_EXIST,        // ���ⲻ����
    CONVERT_TYPE_ERR,       // ת������ʧ��
    END_OF_DATA_SHEET,      // ʹ�� ForRow �����������ݱ�ʱ����������ʱ�᷵�ظ�ֵ
    CSV_FORMAT_ERR,         // CSV ���ݵĸ�ʽ����ȷ
    EMPTY_DATA_SHEET,       // �հױ��
    INCORRECT_TYPE,         // ���Ͳ���ȷ
    /* HAN_Hex */
    HEX_FILE_FORMAT_ERR,    // Hex �ļ���ʽ����
    SREC_FILE_FORMAT_ERR,   // Srec �ļ���ʽ����
    /* HAN_windows */
    CREATE_COM_FILE_ERR,    // ���������ļ����󣬿����Ǵ���������
    SET_COM_QUEUE_ERR,      // ���û�������Сʧ�ܣ���������СӦ��ż��
    SET_COM_TIME_OUT_ERR,   // ���ó�ʱ�ȴ�ʧ��
    SET_COM_STATE_ERR,      // ���ô��ڲ���ʧ�ܣ����鲨���ʣ��ֽڳ��ȣ�У��λ��ֹͣλ�Ƿ��ǺϷ�����
    READ_COM_ERR,           // ��ȡ����ʧ�ܣ����鴮���Ƿ���
    /* multi list */
    HML_EMPTY,              // �༶�б�Ϊ��
    HML_ID_OUT_OF_RANGE,    // ID ������Χ
    HML_NO_CHILD,           // ��ǰ�����еĶ༶�б����ӽڵ�
    /* gdi */
    HCOM_PORT_ID_NOT_EXIST, // �˿ںŲ�����
    HCOM_NOT_OPEN,          // �˿�δ��
    HCOM_WRITING,           // �˿����ڷ�������
    /* plot */
    PLOT_VAR_NOT_EXIST,     // ����������
    CREATE_OBJECT_ERR,      // ��������ʧ��
    CREATE_PEN_ERR,         // ��������ʧ��
    CREATE_BRUSH_ERR,       // ������ˢʧ��
    CREATE_FONT_ERR,        // ��������ʧ��
    HPL_VAR_HPOS_EXIST,     // ������ͼ��ӱ���ʱָ��λ��Ϊ������ײ���ˮƽλ��ʱ����λ���Ѵ��ڱ��������ɼ������
    HPL_RCINVH_ERR,         // ���û�ͼ�����׸߶ȵ�ֵ����
    HST_VAR_POS_ERR,        // ��ɢ��ͼ��������ʱ����λ�ò�ƥ��
    HST_MARK_DIS_ERR,       // ɢ��ͼ���߼�����󣨱������ 0 ��
};

#define ArrLen(arr)     (sizeof(arr) / sizeof(arr[0]))  // ����Ԫ�ظ���
#define StructMemberOffset(structure, member)   ((size_t)(&(((structure*)0)->member)))
#define StructMemberSize(structure, member)     ((size_t)sizeof(((structure*)0)->member))

#ifdef __cplusplus
}
#endif

#endif
