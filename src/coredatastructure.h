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
    Course(){}
    Course(int id , const QString& name);
};

class Mapping {
public:
    Teacher* teacher;
    Course* course;
    int courseid;
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
    /* different teachers with the same name? */
    Teacher* addTeacher(const QString& teacherName, const QString& teacherPinyin);
    Teacher* removeTeacher(const QString& teacherName);

    Course* addCourse(int courseid,const QString& courseName);
    Course* removeCourse(int courseid);

    bool addMapping(Teacher* teacher, Course* course);

};

#endif // COREDATASTRUCTURE_H
