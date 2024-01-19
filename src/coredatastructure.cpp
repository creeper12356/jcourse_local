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

bool CoreData::readFromJsonObject(const QJsonObject &obj)
{
    QJsonArray coursesJsonArray = obj["courses"].toArray();
    QJsonArray teachersJsonArray = obj["teachers"].toArray();
    QJsonArray mappingsJsonArray = obj["mappings"].toArray();
    QMap<int , Course*> courseMap;
    QMap<QString , Teacher*> teacherMap;
    bool ok;

    for(auto it = coursesJsonArray.begin();it != coursesJsonArray.end();++it){
        Course* course = addCourse((*it).toObject()["id"].toInt(),(*it).toObject()["name"].toString(),&ok);
        if(!ok){
            return false;
        }

        //new course added
        course->code = (*it).toObject()["code"].toString();
        course->credit = (*it).toObject()["credit"].toInt();
        course->department = (*it).toObject()["department"].toString();
        course->ratingAvg = (*it).toObject()["ratingAvg"].toDouble();
        course->ratingCount = (*it).toObject()["ratingCount"].toInt();
        //add to course map
        courseMap.insert(course->id,course);
    }
    for(auto it = teachersJsonArray.begin();it != teachersJsonArray.end();++it){
        Teacher* teacher = addTeacher((*it).toObject()["name"].toString());
        if(!ok){
            return false;
        }
        //new teacher added
        teacher->pinyin = (*it).toObject()["pinyin"].toString();
        teacher->abbrPinyin = (*it).toObject()["abbrPinyin"].toString();
        //add to teacher map
        teacherMap.insert(teacher->name,teacher);
    }
    for(auto it = mappingsJsonArray.begin();it != mappingsJsonArray.end();++it){
        Teacher* teacher = teacherMap[(*it).toObject()["teacherName"].toString()];
        Course* course = courseMap[(*it).toObject()["courseid"].toInt()];
        bool ok = addMapping(teacher,course);
        if(!ok){
            return false;
        }
    }
    return true;
}

QJsonObject CoreData::toJsonObject() const
{
    QJsonObject obj;
    QJsonArray coursesJsonArray;
    QJsonArray teachersJsonArray;
    QJsonArray mappingsJsonArray;

    for(Course* course: mCourses){
        QJsonObject courseJsonObject;
        courseJsonObject.insert("id",course->id);
        courseJsonObject.insert("name",course->name);
        courseJsonObject.insert("code",course->code);
        courseJsonObject.insert("credit",course->credit);
        courseJsonObject.insert("department",course->department);
        courseJsonObject.insert("ratingAvg",course->ratingAvg);
        courseJsonObject.insert("ratingCount",course->ratingCount);

        coursesJsonArray.append(courseJsonObject);
    }
    for(Teacher* teacher: mTeachers){
        QJsonObject teacherJsonObject;
        teacherJsonObject.insert("name",teacher->name);
        teacherJsonObject.insert("pinyin",teacher->pinyin);
        teacherJsonObject.insert("abbrPinyin",teacher->abbrPinyin);

        teachersJsonArray.append(teacherJsonObject);
    }
    for(const Mapping& mapping: mMappings){
        QJsonObject mappingJsonObject;
        //教师使用名字为索引
        mappingJsonObject.insert("teacherName",mapping.teacher->name);
        //课程使用id为索引
        mappingJsonObject.insert("courseid",mapping.course->id);

        mappingsJsonArray.append(mappingJsonObject);
    }

    obj.insert("courses",coursesJsonArray);
    obj.insert("teachers",teachersJsonArray);
    obj.insert("mappings",mappingsJsonArray);

    return obj;
}

bool CoreData::readFromFile(const QString &fileName)
{
    QFile reader(fileName);
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        return false;
    }

    bool res = readFromJsonObject(QJsonDocument::fromJson(reader.readAll()).object());
    reader.close();
    return res;

}

void CoreData::writeToFile(const QString &fileName) const
{
    QFile writer(fileName);
    writer.open(QIODevice::WriteOnly);
    writer.write(QJsonDocument(toJsonObject()).toJson());
    writer.close();
}

Teacher *CoreData::addTeacher(const QString &teacherName,bool *ok)
{
    for(Teacher* teacher: mTeachers){
        if(teacher->name == teacherName){
            if(ok){
                *ok = false;
            }
            //已经存在该教师
            return teacher;
        }
    }
    Teacher *newTeacher = new Teacher(teacherName);
    mTeachers.push_back(newTeacher);
    if(ok){
        *ok = true;
    }
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

Course *CoreData::addCourse(int courseid, const QString &courseName,bool* ok)
{
    for(Course* course: mCourses){
        if(course->id == courseid){
            if(ok){
                *ok = false;
            }
            //已经存在该课程
            return course;
        }
    }
    Course *newCourse = new Course(courseid,courseName);
    mCourses.push_back(newCourse);
    if(ok){
        *ok = true;
    }
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
        return false;
    }
    else{
        mMappings.push_back(newMapping);
        return true;
    }

}

QDebug operator<<(QDebug debug, const Teacher &teacher)
{
    debug << "Teacher(" << teacher.name << "," << teacher.pinyin << ")";
    return debug;
}

Teacher::Teacher(const QString &arg_name)
    : name(arg_name)
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
{

}

bool Mapping::operator ==(const Mapping &other) const
{
    return this->teacher == other.teacher && this->course == other.course;
}

QVector<const Mapping*> CoreData::searchCourseMappings(const QString &teacherName,
                                                       const QString &teacherPinyin,
                                                       const QString &teacherAbbrPinyin,
                                                       const QString &courseName,
                                                       const QString &courseCode) {
    QVector<const Mapping*> result;

    // 在数据集中搜索匹配条件的courseId
    for (const Mapping& mapping : mMappings){
        if (mapping.teacher->name.contains(teacherName,Qt::CaseInsensitive) ||
            mapping.teacher->pinyin.toLower() == teacherPinyin.toLower() ||
            mapping.teacher->abbrPinyin.toLower() == teacherAbbrPinyin.toLower() ||
            mapping.course->name.contains(courseName, Qt::CaseInsensitive) ||
            mapping.course->code.toLower() == courseCode.toLower()){

            result.push_back(&mapping);
        }
    }

    return result;
}
