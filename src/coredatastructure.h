#ifndef COREDATASTRUCTURE_H
#define COREDATASTRUCTURE_H
#include "inc.h"
//存储搜索引擎需要的核心数据结构

class Teacher {
    friend QDebug operator<<(QDebug debug, const Teacher& teacher);
public:
    QString name;
    QString pinyin;
    Teacher(){}
    Teacher(const QString& name, const QString& pinyin);

};

class Course {
    friend QDebug operator<<(QDebug debug, const Course& course);
public:
    int id;

    QString name;
    QString code;
    int credit;
    QString department;

    qreal ratingAvg;
    int ratingCount;

    Course(){}
    Course(int id , const QString& name);
};

class Mapping {
public:
    Teacher* teacher;
    Course* course;
    Mapping() {}
    Mapping(Teacher* teacher , Course* course);
    bool operator ==(const Mapping& other) const;
};

class CoreData {

private:
    QVector<Teacher*> mTeachers;
    QVector<Course*> mCourses;
    QVector<Mapping> mMappings;

public:
    CoreData(){}
    ~CoreData();

    bool readFromJsonObject(const QJsonObject& obj);
    QJsonObject toJsonObject() const;
    //从文件fileName中读取，返回是否读取成功
    bool readFromFile(const QString& fileName);
    //将所有资源写入文件fileName
    void writeToFile(const QString& fileName) const;

    //TODO : 重名的教师？
    //添加新教师，通过ok指针返回是否成功
    //*ok == true : 添加成功，返回指向该教师的指针
    //*ok == false : 添加失败，教师已存在，返回已存在的教师指针
    Teacher* addTeacher(const QString& teacherName, const QString& teacherPinyin,bool* ok = nullptr);
    Teacher* removeTeacher(const QString& teacherName);

    //添加新课程，通过ok指针返回是否成功
    //*ok == true : 添加成功，返回指向该课程的指针
    //*ok == false : 添加失败，课程已存在，返回已存在课程的指针
    Course* addCourse(int courseid,const QString& courseName,bool* ok = nullptr);
    Course* removeCourse(int courseid);

    //添加教师和课程的映射，返回是否添加成功
    bool addMapping(Teacher* teacher, Course* course);

    //搜索函数，返回所有符合条件的映射
    QVector<const Mapping*> searchCourseMappings(const QString &teacherName, const QString &teacherPinyin, const QString &courseName);

};

#endif // COREDATASTRUCTURE_H
