#include "coredatastructure.h"


CoreData::~CoreData()
{
    for(auto teacher: mTeachers){
        delete teacher;
    }
    for(auto course: mCourses){
        delete course;
    }
}

Teacher *CoreData::addTeacher(const QString &teacherName, const QString &teacherPinyin)
{
    for(Teacher* teacher: mTeachers){
        if(teacher->name == teacherName){
            //已经存在该教师
            return nullptr;
        }
    }
    Teacher *newTeacher = new Teacher(teacherName,teacherPinyin);
    mTeachers.push_back(newTeacher);
    return newTeacher;
}

Teacher *CoreData::removeTeacher(const QString &teacherName)
{
    for(auto it = mTeachers.begin();it != mTeachers.end();++it){
        if((*it)->name == teacherName){
            return mTeachers.takeAt(it - mTeachers.begin());
        }
    }
    return nullptr;
}

Course *CoreData::addCourse(int courseid, const QString &courseName)
{
    for(Course* course: mCourses){
        if(course->id == courseid){
            //已经存在该课程
            return nullptr;
        }
    }
    Course *newCourse = new Course(courseid,courseName);
    mCourses.push_back(newCourse);
    return newCourse;
}

Course *CoreData::removeCourse(int courseid)
{
    for(auto it = mCourses.begin();it != mCourses.end();++it){
        if((*it)->id == courseid){
            return mCourses.takeAt(it - mCourses.begin());
        }
    }
    return nullptr;
}

bool CoreData::addMapping(Teacher *teacher, Course *course)
{
    Mapping newMapping(teacher,course);
    if(mMappings.contains(newMapping)){
        qDebug() << "already mapped";
        return false;
    }
    else{
        mMappings.push_back(newMapping);
        qDebug() << "add Mapping : " << *teacher << *course;
        return true;
    }

}

QDebug operator<<(QDebug debug, const Teacher &teacher)
{
    debug << "Teacher(" << teacher.name << "," << teacher.pinyin << ")";
    return debug;
}

Teacher::Teacher(const QString &arg_name, const QString &arg_pinyin)
    : name(arg_name)
    , pinyin(arg_pinyin)
{

}

QDebug operator<<(QDebug debug, const Course &course)
{
    debug << "Course(" << course.id << "," << course.name << ")";
    return debug;
}

Course::Course(int arg_id, const QString &arg_name)
    : id(arg_id)
    , name(arg_name)
{

}

Mapping::Mapping(Teacher *arg_teacher, Course *arg_course)
    : teacher(arg_teacher)
    , course(arg_course)
    , courseid(arg_course->id)
{

}

bool Mapping::operator ==(const Mapping &other) const
{
    return this->teacher == other.teacher && this->course == other.course;
}
