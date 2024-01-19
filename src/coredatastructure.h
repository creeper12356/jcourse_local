#ifndef COREDATASTRUCTURE_H
#define COREDATASTRUCTURE_H
#include "inc.h"
/*!
 * \brief 教师类
 */
class Teacher {

    friend QDebug operator<<(QDebug debug, const Teacher& teacher);
public:
    QString name; /*!<教师姓名，目前是教师的唯一标识符（暂时不考虑重名）*/
    QString pinyin; /*!<教师姓名全拼（每个字首字母大写,e.g: ZhangSan）*/
    QString abbrPinyin; /*!<教师姓名拼音缩写 */
    Teacher(){}
    Teacher(const QString& name);
};

/*!
 * \brief 课程类
 */
class Course {
    friend QDebug operator<<(QDebug debug, const Course& course);
public:
    /*!
     * 课程id，课程的唯一标识符，与选课社区网站上的course id对应。
     */
    int id;

    QString name;
    QString code;
    qreal credit;
    QString department;

    qreal ratingAvg;
    int ratingCount;

    Course(){}
    Course(int id , const QString& name);
};
/*!
 * \brief 包装类，表示教师和课程的关系
 *
 * Mapping类用于表示教师与课程的关系，如果教师A（使用Teacher类对象teacherA表示）讲授课程B（使用Course类对象courseB表示），
 * 那么这种关系可以表示为Mapping(&teacherA,&courseB)。
 */
class Mapping {
public:
    Teacher* teacher;
    Course* course;
    Mapping() {}
    Mapping(Teacher* teacher , Course* course);

    /*!
     * \brief 重载==运算符
     */
    bool operator ==(const Mapping& other) const;
};

/*!
 * \brief 搜索引擎相关的核心数据结构
 *
 * 所有教师、课程、教师和课程的对应关系都存储在此数据结构中，这是一个自己实现的数据库（目前使用线性搜索，效率较低），
 * 实现了基本的增删改查。
 */

class CoreData {
public:
    CoreData(){}
    ~CoreData();
public:
    /*!
     * \brief 按照关键字搜索课程
     *
     * 按照关键字搜索教师-课程的对应关系，
     * 目前支持的搜索方式有：教师姓名模糊搜索，教师拼音精确搜索，教师拼音缩写精确搜索，课程名称模糊搜索，课号精确搜索。
     * \param teacherName 教师姓名搜索关键字
     * \param teacherPinyin 教师拼音搜索关键字
     * \param teacherAbbrPinyin 教师拼音缩写搜索关键字
     * \param courseName 课程名称搜索关键字
     * \param courseCode 课号关键字
     * \return 所有符合搜索要求的教师-课程关系，存储在一个QVector容器中。
     */
    QVector<const Mapping*> searchCourseMappings(const QString &teacherName,
                                                 const QString &teacherPinyin,
                                                 const QString &teacherAbbrPinyin,
                                                 const QString &courseName,
                                                 const QString &courseCode);

    /*!
     * \brief 从QJsonObject对象中读取信息
     * \param obj 读取的QJsonObject对象
     * \return 如果读取正常，返回true；如果读取过程出现错误，返回false。
     * TODO：此函数目前不完善，读取obj之前没有检查对应的键是否存在。
     */
    bool readFromJsonObject(const QJsonObject& obj);

    /*!
     * \brief 转换为QJsonObject对象
     */
    QJsonObject toJsonObject() const;

    /*!
     * \sa readFromJsonObject
     * \brief 从文件中读取信息
     * \param fileName 读取文件的路径
     * \return 读取是否成功
     */
    bool readFromFile(const QString& fileName);

    /*!
     * \sa toJsonObject
     * \brief 将当前信息写入文件
     * \param fileName 写入文件的路径
     */
    void writeToFile(const QString& fileName) const;

    /*!
     * \brief 添加新的教师
     * \param teacherName 新教师姓名
     * \param ok 是否添加成功
     * \return 如果数据库中不存在姓名为{teacherName}的教师，将该教师添加到库中，并返回指向新教师的指针；
     * 如果数据库中已经存在姓名为{teacherName}的教师，直接返回指向库中的该教师的指针。
     * 如果参数ok不为nullptr,添加教师是否成功将被写入*ok中。
     */
    Teacher* addTeacher(const QString& teacherName,bool* ok = nullptr);

    /*!
     * \brief 删除教师
     * \param teacherName 删除教师的姓名
     * \return 如果数据库中存在姓名为{teacherName}的教师，将该教师从数据库中移除，返回指向该教师的指针，
     * 该指针指向内存需要调用者手动释放；
     * 如果数据库中不存在姓名为{teacherName}的教师，返回nullptr。
     */
    Teacher* removeTeacher(const QString& teacherName);

    /*!
     * \brief 添加新的课程
     * \param courseid 课程id
     * \param courseName 课程名
     * \param ok 是否添加成功
     * \return 如果数据库中不存在id为{courseid}的课程，将该课程添加到库中，并返回指向新课程的指针；
     * 如果数据库中已经存在id为{courseid}的教师，直接返回指向库中的该课程的指针。
     * 如果参数ok不为nullptr,添加课程是否成功将被写入*ok中。
     */
    Course* addCourse(int courseid,const QString& courseName,bool* ok = nullptr);

    /*!
     * \brief 删除课程
     * \param courseid 删除课程的id
     * \return 如果数据库中存在id为{courseid}的教师，将该课程从数据库中移除，返回指向该课程的指针，
     * 该指针指向内存需要调用者手动释放；
     * 如果数据库中不存在id为{courseid}的课程，返回nullptr。
     */
    Course* removeCourse(int courseid);

    /*!
     * \brief 添加一组教师和课程的关系
     * \param teacher 指向教师的指针
     * \param course 指向课程的指针
     * \return 如果不存在相同的关系，添加成功，返回true；
     * 如果已经存在相同的关系，添加失败，返回false。
     */
    bool addMapping(Teacher* teacher, Course* course);

private:
    QVector<Teacher*> mTeachers;
    QVector<Course*> mCourses;
    QVector<Mapping> mMappings;

};

#endif // COREDATASTRUCTURE_H
