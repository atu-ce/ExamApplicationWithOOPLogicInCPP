#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include <algorithm>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace std;

bool isStarted = false;
string globalUsername = "";

void registrationFailed()
{
    isStarted = false;
}

void userCreated()
{
    isStarted = true;
}

string trim(const string &str, const string &whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

string toLowercase(string data)
{
    for_each(data.begin(), data.end(), [](char &c)
             { c = ::tolower(c); });
    return data;
}

string get_current_dir()
{
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

bool isPathExist(const string &s)
{
    struct stat buffer;
    return (stat(s.c_str(), &buffer) == 0);
}

string passwordCheck(int firstPassword, int secondPassword)
{
    int counter = 0;
    if (firstPassword != secondPassword)
    {
        while (true)
        {
            if (counter == 22)
            {
                cout << "3 defa yanlis parola girdiniz!" << endl;
                return "failed";
            }
            cout << "Parola uyusmuyor! Lutfen tekrar giriniz.\nParola: ";
            cin >> secondPassword;
            if (firstPassword == secondPassword)
                return "successful";
            counter++;
        }
    }
    return "successful";
}

class Questions
{
private:
    string answer;

protected:
    string text;

public:
    Questions(string comText, string comAnswer)
    {
        text = comText;
        answer = comAnswer;
    }

    // Kullanıcının verdiği cevabın doğruluğunu ölçer.
    bool checkAnswer(string givenAnswer)
    {
        return givenAnswer == answer;
    }

    string getAnswer()
    {
        return answer;
    }

    string getQuestion()
    {
        return text;
    }
};

class Quiz
{
private:
    vector<string> trueAnswers, falseAnswers;

protected:
    float score;
    int totalTrueAnswer, totalFalseAnswer;
    vector<Questions> questions;
    int questionIndex;

public:
    Quiz(vector<Questions> comQuestions)
    {
        questions = comQuestions;
        score = 0.0;
        questionIndex = 0;
        totalTrueAnswer = 0;
        totalFalseAnswer = 0;
        trueAnswers = {};
        falseAnswers = {};
    }

    // Bu fonksiyonun amacı, o anki 'questionIndex'in değerine karşılık gelen  question objesini göndermek.
    Questions getQuestion()
    {
        return questions[questionIndex];
    }

    // Bu fonksiyon, gelen question objesini ekrana yazdırır.
    void disPlayQuestion(string course)
    {
        Questions question = getQuestion();
        string answer = "";

        string bug = "";
        getline(cin, bug); // getline() fonksiyonunun bir bug'ından dolayı bu satır yazılmıştır.
        cout << "Soru " << (questionIndex + 1) << ": " << question.getQuestion() << "?\nCevap: ";
        getline(cin, answer);
        answer = toLowercase(answer);
        answer = trim(answer);
        guess(answer);
        loadQuestion(course);
    }

    // Bu fonksiyonun amacı, koşulsuz index numarasını arttırmak ve
    // 'checkAnswer'dan gelen değer 'True' ise totalTrueAnswer değiskenini arttırmak.
    void guess(string answer)
    {
        Questions question = getQuestion();

        if (question.checkAnswer(answer))
        {
            totalTrueAnswer++;
            isTrue(answer);
        }
        else
        {
            totalFalseAnswer++;
            isFalse(answer);
        }
        questionIndex++;
    }

    // İndex aşımını kontrol eder.
    void loadQuestion(string course)
    {
        if (questions.size() == questionIndex)
        {
            showScore();
            createQuizResults(course);
        }
        else
        {
            disPlayProgress();
            disPlayQuestion(course);
        }
    }

    // Quizi bitirir ve kullanıcıya skorunu gösterir.
    void showScore()
    {
        int totalQuestion = totalTrueAnswer + totalFalseAnswer;
        score = (totalTrueAnswer * 100) / totalQuestion;

        printf("\n");
        cout << string(40, '*') << " Sinav Bitti " << string(47, '*') << endl;
        printf("\n");
        cout << "Sonuclar:" << endl;
        cout << " Notunuz: " << score << endl;

        printf("\n Dogru Cevaplariniz:\n");
        for (int i = 0; i < totalTrueAnswer; i++)
        {
            cout << "  > " << trueAnswers[i] << endl;
        }
        if (totalTrueAnswer == 0)
            cout << "  > Dogru cevabiniz yok." << endl;

        printf("\n Yanlis Cevaplariniz:\n");
        for (int j = 0; j < totalFalseAnswer; j++)
        {
            cout << "  > " << falseAnswers[j] << endl;
        }
        if (totalFalseAnswer == 0)
            cout << "  > Yanlis cevabiniz yok." << endl;

        printf("\n");
        cout << string(100, '*') << endl;
    }

    // Kullanıcıya kaç sorudan kaçıncıda olduğunu gösterir.
    void disPlayProgress()
    {
        int totalQuestion = questions.size();
        int questionNumber = questionIndex + 1;

        printf("\n");
        if (totalQuestion < 10)
            cout << string(40, '*') << " Soru " << questionNumber << "/" << totalQuestion << " " << string(50, '*') << endl;
        else
        {
            if (questionNumber < 10)
                cout << string(40, '*') << " Soru " << questionNumber << "/" << totalQuestion << " " << string(49, '*') << endl;
            else
                cout << string(40, '*') << " Soru " << questionNumber << "/" << totalQuestion << " " << string(48, '*') << endl;
        }
        printf("\n");
    }

    // Kullanıcıya hangi soruya doğru, hangisine yanlış cevap verdiğini göstermeliyiz;
    // ** Doğru ise bu fonksiyon çalışır.
    void isTrue(string answer)
    {
        ostringstream creatingText;
        creatingText << questionIndex + 1 << ". soru (" << answer << ").";
        string result = creatingText.str();
        trueAnswers.push_back(result);
    }

    // ** Yanlış ise bu fonksiyon çalışır.
    void isFalse(string answer)
    {
        Questions question = getQuestion();
        string trueAnswer = question.getAnswer();
        ostringstream creatingText;
        creatingText << questionIndex + 1 << ". soru (" << answer << "), dogru cevap: " << trueAnswer << ".";
        string result = creatingText.str();
        falseAnswers.push_back(result);
    }

    // Sınav bitiminde bu fonksiyon çalışır, giriş yapan kullanıcının klasörüne o anki 'tarih-saat.txt' dosyası oluşturulur ve doğru-yanlış cevap tablosu dosyaya eklenir.
    void createQuizResults(string course)
    {
        string createdDate, createdTime, createdDateTime;

        time_t now = time(0);
        tm *ltm = localtime(&now);

        createdDate = to_string(ltm->tm_mday) + "_" + to_string(1 + ltm->tm_mon) + "_" + to_string(1900 + ltm->tm_year);
        createdTime = to_string(ltm->tm_hour) + "_" + to_string(ltm->tm_min) + "_" + to_string(ltm->tm_sec);
        createdDateTime = createdDate + "_" + createdTime;

        string way = get_current_dir() + "/users/" + globalUsername + "/" + course;
        bool isCourseFolder = isPathExist(way);

        if (!isCourseFolder)
        {
            int status = mkdir(way.c_str(), 0777);
        }
        string resultsWay = way + "/" + createdDateTime + ".txt";

        ofstream dateTimeFile(resultsWay);
        dateTimeFile << "Notunuz: " << score << endl;
        dateTimeFile << "\nDogru Cevaplariniz:" << endl;
        for (int i = 0; i < totalTrueAnswer; i++)
        {
            dateTimeFile << " > " << trueAnswers[i] << endl;
        }
        if (totalTrueAnswer == 0)
            dateTimeFile << " > Dogru cevabiniz yok." << endl;
        dateTimeFile << "\n";
        dateTimeFile << "Yanlis Cevaplariniz:" << endl;
        for (int j = 0; j < totalFalseAnswer; j++)
        {
            dateTimeFile << " > " << falseAnswers[j] << endl;
        }
        if (totalFalseAnswer == 0)
            dateTimeFile << " > Yanlis cevabiniz yok." << endl;
        dateTimeFile.close();
    }
};

class People
{
protected:
    string name, surname, gender;
    int age;

public:
    People();
};

People::People()
{
    name = "";
    surname = "";
    gender = "";
    age = 0;
}
class Users : public People
{
protected:
    int password, passwordAgain;
    string username, role;

public:
    string title;

    Users()
    {
        name = "";
        surname = "";
        gender = "";
        age = 0;
        username = "";
        title = "";
        role = "";
        password = 0;
        passwordAgain = 0;
    }

    // Ana sınıfın signUp() fonksiyonudur kaydolma işlemleri türetilmiş sınıfların signUp() fonksiyonunda yapılacaktır.
    virtual bool signUp()
    {
        string way = get_current_dir();
        string usersWay = way + "/users";

        bool isUsersFolder = isPathExist(usersWay);

        if (!isUsersFolder)
        {
            int status = mkdir(usersWay.c_str(), 0777);
        }

        return false;
    }

    bool signIn()
    {
        string localUsername = "";
        int localPassword = 0;

        cout << "Kullanici adi: ";
        cin >> localUsername;
        localUsername = trim(localUsername);
        localUsername = toLowercase(localUsername);

        string way = get_current_dir();
        string loginInquiryWay = way + "/loginInquiry.txt";

        bool isloginInquiryFile = isPathExist(loginInquiryWay);

        if (!isloginInquiryFile)
        {
            ofstream loginInquiryFile(loginInquiryWay);
            loginInquiryFile.close();
        }

        string fileContent = "";
        ifstream readLoginInquiryFile(loginInquiryWay);

        while (getline(readLoginInquiryFile, fileContent))
        {
            if (localUsername == fileContent)
            {
                cout << "Parola: ";
                cin >> localPassword;

                string way = get_current_dir();
                string userProfileWay = way + "/users/" + localUsername + "/profile.txt";

                string profileContent = "";
                ifstream readUserProfile(userProfileWay);
                string delimiter = ": ";

                while (getline(readUserProfile, profileContent))
                {
                    string token = profileContent.substr(0, profileContent.find(delimiter));
                    if (token == "Parola")
                    {
                        int passwordByDataBase = stoi(profileContent.erase(0, profileContent.find(delimiter) + delimiter.length()));
                        string result = passwordCheck(passwordByDataBase, localPassword);
                        if (result == "failed")
                        {
                            cout << "Cok fazla hatali giris yaptiniz! Kullanici adinizdan ve parolanizdan emin olunuz." << endl;
                            return false;
                        }
                        cout << "Giris Basarili." << endl;
                        break;
                    }
                }
                isStarted = true;
                globalUsername = localUsername;
                break;
            }
        }
        readLoginInquiryFile.close();

        if (globalUsername != localUsername)
        {
            cout << "Girmis oldugunuz '" << localUsername << "' adinda kullanici sistemde kayitli degildir!" << endl;
            return false;
        }
        return true;
    }
};

class Lessons
{
protected:
    string letterGrade;
    float average;
    vector<int> notes;

public:
    string courseName;
    int currentClass;
    friend string isPassed(Lessons lessons);

    Lessons(string comCourseName, int comCurrentClass, int comTotalExams, int comFinishedExams)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = comTotalExams;
        finishedExams = comFinishedExams;
        letterGrade = "";
        average = 0.0;
        notes = {};
    }
    Lessons(string comCourseName, int comCurrentClass, int comTotalExams)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = comTotalExams;
        finishedExams = 0;
        letterGrade = "";
        average = 0.0;
        notes = {};
    }
    Lessons(string comCourseName, int comCurrentClass)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = 2;
        finishedExams = 0;
        letterGrade = "";
        average = 0.0;
        notes = {};
    }
};

vector<Lessons> createLessonObjects(int whichGrader, string role);

class Students : public Users
{
protected:
    int whichGrader;
    int totalLessons;
    vector<Lessons> lessons;

public:
    Students();

    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve bilgiler profile.txt dosyasına kaydedilir.
    bool signUp()
    {
        cout << "Sistemimiz asagida listelenmis bolumleri kapsamaktadir," << endl;
        cout << "1.) Bilgisayar Muhendisligi\n2.) Makine Muhendisligi" << endl;
        string branch = "";
        while (true)
        {
            cout << "Lutfen bolumunuzu seciniz: ";
            cin >> branch;
            if (branch == "1")
            {
                role = "BM";
                break;
            }
            else if (branch == "2")
            {
                role = "MM";
                break;
            }
            else
                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
        }

        while (true)
        {
            cout << "Kacinci sinifsiniz: ";
            cin >> whichGrader;
            if (whichGrader == 1 || whichGrader == 2 || whichGrader == 3 || whichGrader == 4)
                break;
            else
                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
        }

        lessons = createLessonObjects(whichGrader, role);
        totalLessons = lessons.size();

        getline(cin, branch); // getline() fonksiyonunun bir bug'ından dolayı bu satır yazılmıştır.
        cout << "\n";
        cout << "Adiniz: ";
        getline(cin, name);
        name = trim(name);
        name = toLowercase(name);

        cout << "SoyAdiniz: ";
        getline(cin, surname);
        surname = trim(surname);
        surname = toLowercase(surname);

        cout << "Cinsiyetiniz: ";
        cin >> gender;
        gender = trim(gender);
        gender = toLowercase(gender);

        cout << "Yasiniz: ";
        cin >> age;

        cout << "Talep ettiginiz kullanici adi: ";
        cin >> username;
        username = trim(username);
        username = toLowercase(username);

        cout << "Parola: ";
        cin >> password;

        cout << "Parola tekrar: ";
        cin >> passwordAgain;

        string loginStatus = passwordCheck(password, passwordAgain);
        if (loginStatus == "failed")
        {
            cout << "Hatali parola girislerinden dolayi '" << username << "' kullanicisi olusturulamadi! Lutfen tekrar deneyiniz." << endl;
            return false;
        }

        string way = get_current_dir();
        string usersWay = way + "/users";

        bool isUsersFolder = isPathExist(usersWay);

        if (!isUsersFolder)
        {
            int status = mkdir(usersWay.c_str(), 0777);
        }

        string path = usersWay + "/" + username;

        try
        {
            int status = mkdir(path.c_str(), 0777);
            if (status == -1)
            {
                throw;
            }
            else
            {
                cout << "'" << username << "' kullanicisi basariyla olusturuldu. Sisteme giris yaparak islemlerinizi gerceklestirebilirsiniz." << endl;

                string userProfileWay = path + "/profile.txt";
                ofstream userProfile(userProfileWay);
                userProfile << "Kullanici Adi: " << username << endl;
                userProfile << "Parola: " << password << endl;
                userProfile << "Isim: " << name << endl;
                userProfile << "Soyisim: " << surname << endl;
                userProfile << "Cinsiyet: " << gender << endl;
                userProfile << "Yas: " << age << endl;
                userProfile << "Unvan: " << title << endl;
                userProfile << "Bolum: " << role << endl;
                userProfile << "Sinif: " << whichGrader << ". Sinif" << endl;
                userProfile << "Toplam Ders Sayisi: " << totalLessons << endl;
                userProfile << "Dersleri: ";
                for (int i = 0; i < lessons.size(); i++)
                {
                    userProfile << lessons[i].courseName << ", ";
                }
                userProfile.close();

                string loginInquiryWay = way + "/loginInquiry.txt";
                ofstream loginInquiryFile;
                loginInquiryFile.open(loginInquiryWay, ios_base::app);
                loginInquiryFile << username << endl;
                loginInquiryFile.close();

                userCreated();
                return true;
            }
        }
        catch (const exception &e)
        {
            cerr << "'" << username << "' kullanicisi '" << e.what() << "' nedeninden dolayi olusturulamadi! Lutfen tekrar deneyiniz." << endl;
            registrationFailed();
        }
        return false;
    }
};

Students::Students()
{
    name = "";
    surname = "";
    gender = "";
    age = 0;
    username = "";
    title = "";
    role = "";
    password = 0;
    passwordAgain = 0;
    whichGrader = 0;
    totalLessons = 0;
    lessons = {};
}

class Teachers : public Users
{
private:
    int passwordForTeacherByClass;

public:
    Teachers()
    {
        passwordForTeacherByClass = 112233;
        name = "";
        surname = "";
        gender = "";
        age = 0;
        username = "";
        title = "";
        role = "";
        password = 0;
        passwordAgain = 0;
    }

    int getPasswordForTeacher()
    {
        return passwordForTeacherByClass;
    }

    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve bilgiler profile.txt dosyasına kaydedilir.
    bool signUp()
    {
        cout << "Sistemimiz asagida listelenmis branslari kapsamaktadir," << endl;
        cout << "1.) Matematik\n2.) Fizik\n3.) Bilgisayar Bolumu Ogretmeni\n4.) Makine Bolumu Ogretmeni" << endl;
        string branch = "";
        while (true)
        {
            cout << "Lutfen bransinizi seciniz: ";
            cin >> branch;
            if (branch == "1")
            {
                role = "Matematik";
                break;
            }
            else if (branch == "2")
            {
                role = "Fizik";
                break;
            }
            else if (branch == "3")
            {
                role = "Bilgisayar";
                break;
            }
            else if (branch == "4")
            {
                role = "Makine";
                break;
            }
            else
                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
        }
        getline(cin, branch); // getline() fonksiyonunun bir bug'ından dolayı bu satır yazılmıştır.
        cout << "\n";
        cout << "Adiniz: ";
        getline(cin, name);
        name = trim(name);
        name = toLowercase(name);

        cout << "SoyAdiniz: ";
        getline(cin, surname);
        surname = trim(surname);
        surname = toLowercase(surname);

        cout << "Cinsiyetiniz: ";
        cin >> gender;
        gender = trim(gender);
        gender = toLowercase(gender);

        cout << "Yasiniz: ";
        cin >> age;

        cout << "Talep ettiginiz kullanici adi: ";
        cin >> username;
        username = trim(username);
        username = toLowercase(username);

        cout << "Parola: ";
        cin >> password;

        cout << "Parola tekrar: ";
        cin >> passwordAgain;

        string loginStatus = passwordCheck(password, passwordAgain);
        if (loginStatus == "failed")
        {
            cout << "Hatali parola girislerinden dolayi '" << username << "' kullanicisi olusturulamadi! Lutfen tekrar deneyiniz." << endl;
            return false;
        }

        string way = get_current_dir();
        string usersWay = way + "/users";

        bool isUsersFolder = isPathExist(usersWay);

        if (!isUsersFolder)
        {
            int status = mkdir(usersWay.c_str(), 0777);
        }

        string path = usersWay + "/" + username;

        try
        {
            int status = mkdir(path.c_str(), 0777);
            if (status == -1)
            {
                throw;
            }
            else
            {
                cout << "'" << username << "' kullanicisi basariyla olusturuldu. Sisteme giris yaparak islemlerinizi gerceklestirebilirsiniz." << endl;

                string userProfileWay = path + "/profile.txt";
                ofstream userProfile(userProfileWay);
                userProfile << "Kullanici Adi: " << username << endl;
                userProfile << "Parola: " << password << endl;
                userProfile << "Isim: " << name << endl;
                userProfile << "Soyisim: " << surname << endl;
                userProfile << "Cinsiyet: " << gender << endl;
                userProfile << "Yas: " << age << endl;
                userProfile << "Unvan: " << title << endl;
                userProfile << "Brans: " << role << endl;
                userProfile.close();

                string loginInquiryWay = way + "/loginInquiry.txt";
                ofstream loginInquiryFile;
                loginInquiryFile.open(loginInquiryWay, ios_base::app);
                loginInquiryFile << username << endl;
                loginInquiryFile.close();

                userCreated();
                return true;
            }
        }
        catch (const exception &e)
        {
            cerr << "'" << username << "' kullanicisi '" << e.what() << "' nedeninden dolayi olusturulamadi! Lutfen tekrar deneyiniz." << endl;
            registrationFailed();
        }
        return false;
    }
};

string isPassed(Lessons lessons)
{
    if (lessons.letterGrade == "FF")
        return "kaldi";
    else if (lessons.letterGrade == "CD" || lessons.letterGrade == "DC" || lessons.letterGrade == "DD")
    {
        // SONRA HESAPLANACAK!
    }
    return "gecti";
}

// Parametre olarak verilen sınıf numarasına göre o sınıfın derslerini döndüren fonksiyon.
vector<string> findLessons(int whichGrader, string branch)
{
    int counter = 0;
    vector<string> lessons = {};

    string way = get_current_dir() + "/allLessons/";
    string departmentLessonsWay = way + branch + "Lessons.txt";

    string fileContent = "";
    string delimiter = ">>>-----<<<";
    ifstream readLessonsFile(departmentLessonsWay);

    while (getline(readLessonsFile, fileContent))
    {
        if (counter == whichGrader)
            break;

        if (fileContent == "1. Sinif Dersleri:" || fileContent == "2. Sinif Dersleri:" || fileContent == "3. Sinif Dersleri:" || fileContent == "4. Sinif Dersleri:")
            continue;

        if (fileContent == delimiter)
            counter++;
        else if (counter == whichGrader - 1)
            lessons.push_back(fileContent);
    }
    readLessonsFile.close();

    return lessons;
}

// Gönderilen branş bilgisine göre o branşa ait dersleri döndüren fonksiyon.
vector<string> findLessons(string branch)
{
    vector<string> lessons = {};

    string way = get_current_dir() + "/allLessons/branches/";
    string branchLessonsWay = way + branch + "Lessons.txt";

    string fileContent = "";
    ifstream readLessonsFile(branchLessonsWay);

    while (getline(readLessonsFile, fileContent))
    {
        int counter = 0;
        for (int i = 0; i < lessons.size(); i++)
        {
            if (lessons[i] != fileContent)
                counter++;
        }
        if (counter == lessons.size())
            lessons.push_back(fileContent);
    }
    readLessonsFile.close();

    return lessons;
}

// String bir ders listesini Lessons sınıfının objesine çeviren fonksiyon.
vector<Lessons> createLessonObjects(int whichGrader, string role)
{
    vector<string> lessons = findLessons(whichGrader, role);
    int lessonsSize = lessons.size();
    vector<Lessons> lessonObjects = {};
    for (string lesson : lessons)
    {
        Lessons lessonObject = Lessons(lesson, whichGrader);
        lessonObjects.push_back(lessonObject);
    }
    return lessonObjects;
}

// 1'e basıldığında bu fonksiyon çalışır. Branşa göre yeni soru ekler.
void questionAppend(string course)
{
    string way = get_current_dir();

    string allQuestionsFolderWay = way + "/allQuestionsAndAnswers";
    bool isAllQuestionFolder = isPathExist(allQuestionsFolderWay);
    if (!isAllQuestionFolder)
    {
        int status = mkdir(allQuestionsFolderWay.c_str(), 0777);
    }

    string courseFolderWay = allQuestionsFolderWay + "/" + course;
    bool isCourseFolder = isPathExist(courseFolderWay);
    if (!isCourseFolder)
    {
        int status = mkdir(courseFolderWay.c_str(), 0777);
    }
    string questionsAndAnswersWay = courseFolderWay + "/questionsAndAnswers.txt";
    ofstream questionsAndAnswersFile;

    while (true)
    {
        string question = "", answer = "";
        string bug = "";
        getline(cin, bug); // getline() fonksiyonunun bir bug'ından dolayı bu satır yazılmıştır.
        cout << "Soru: ";
        getline(cin, question);
        question = trim(question);
        question = toLowercase(question);

        cout << "Cevap: ";
        getline(cin, answer);
        answer = trim(answer);
        answer = toLowercase(answer);

        questionsAndAnswersFile.open(questionsAndAnswersWay, ios_base::app);
        questionsAndAnswersFile << question << ": " << answer << endl;
        questionsAndAnswersFile.close();

        string choice = "";
        cout << "Baska soru eklenecek mi? (evet/e veya hayir/h)\n";
        while (true)
        {
            cout << "Tercih: ";
            cin >> choice;
            if (choice == "evet" || choice == "e" || choice == "hayir" || choice == "h")
                break;
            else
                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
        }
        if (choice == "evet" || choice == "e")
            continue;
        else if (choice == "hayir" || choice == "h")
            break;
    }
}

// 2'ye basıldığında bu fonksiyon çalışır. Sınavı başlatır.
void quizStart(string course)
{
    vector<Questions> questions = {};
    string way = get_current_dir();

    string allQuestionsFolderWay = way + "/allQuestionsAndAnswers";
    bool isAllQuestionFolder = isPathExist(allQuestionsFolderWay);
    if (!isAllQuestionFolder)
    {
        int status = mkdir(allQuestionsFolderWay.c_str(), 0777);
    }

    string courseFolderWay = allQuestionsFolderWay + "/" + course;
    bool isCourseFolder = isPathExist(courseFolderWay);
    if (!isCourseFolder)
    {
        int status = mkdir(courseFolderWay.c_str(), 0777);
    }

    string questionsAndAnswersWay = courseFolderWay + "/questionsAndAnswers.txt";
    bool isQuestionsAndAnswersFile = isPathExist(questionsAndAnswersWay);
    if (!isQuestionsAndAnswersFile)
    {
        ofstream loginInquiryFile(questionsAndAnswersWay);
        loginInquiryFile.close();
    }

    string fileContent = "";
    string delimiter = ": ";
    ifstream readQuestionsAndAnswersFile(questionsAndAnswersWay);

    while (getline(readQuestionsAndAnswersFile, fileContent))
    {
        string token = fileContent.substr(0, fileContent.find(delimiter));
        fileContent.erase(0, fileContent.find(delimiter) + delimiter.length());
        questions.push_back(Questions(token, fileContent));
    }

    readQuestionsAndAnswersFile.close();

    if (questions.size() == 0)
    {
        cout << "Secmis oldugunuz '" << course << "' derse ait bir sinav bulunmamaktadir!" << endl;
    }
    else
    {
        Quiz quiz(questions);
        quiz.loadQuestion(course);
    }
}

// Menünün oluşturulduğu bölüm
void createMenu()
{
    /*
        Menü Aşağıdaki gibi olmalı!
        > Kayıt Ol
            Bilgiler alınır tekrar ana menüye dönülür.
        > Giriş Yap
            Gİriş başarılı ise,
            Öğretmen için;
                > Soru Ekle
                > Ders Başarı Raporunu Görüntüle
                > Ana Menü
            Öğrenci için;
            > Sınav Ol
            > Sonuçları Gör
            > Ana Menü
        > Çıkış
    */
    int selectedAction = 0;
    while (true)
    {
        cout << "\n1.) Soru Ekle\n2.) Sinava Basla\n3.) Cikis\nTercihiniz: ";
        cin >> selectedAction;

        if (selectedAction == 1)
        {
            string access = "";
            int password = 0;
            bool isUser = false;

            // Soru eklemeyi sadece öğretmenlerin yapabilmesi için gerekli kod dizini.
            cout << "Kullanici adi: ";
            cin >> access;
            access = trim(access);
            access = toLowercase(access);

            string way = get_current_dir();
            string loginInquiryWay = way + "/loginInquiry.txt";

            bool isloginInquiryFile = isPathExist(loginInquiryWay);

            if (!isloginInquiryFile)
            {
                ofstream loginInquiryFile(loginInquiryWay);
                loginInquiryFile.close();
            }

            string fileContent = "";
            ifstream readLoginInquiryFile(loginInquiryWay);

            while (getline(readLoginInquiryFile, fileContent))
            {
                if (access == fileContent)
                {
                    isUser = true;
                    break;
                }
            }
            readLoginInquiryFile.close();
            if (!isUser)
            {
                cout << "Girmis oldugunuz '" << access << "' adli kullanici sisteme kayitli degildir! Kullanici adinizdan emin olunuz." << endl;
                continue;
            }

            string userProfileWay = way + "/users/" + access + "/profile.txt";

            string profileContent = "", branch = "", course = "";
            ifstream readUserProfile(userProfileWay);
            string delimiter = ": ";
            bool isTeacher = false;
            int passwordByDataBase = 0;

            while (getline(readUserProfile, profileContent))
            {
                string token = profileContent.substr(0, profileContent.find(delimiter));
                if (token == "Parola")
                {
                    profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                    passwordByDataBase = stoi(profileContent);
                    continue;
                }
                if (token == "Unvan")
                {
                    profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                    if (profileContent == "Ogretmen")
                        isTeacher = true;
                    continue;
                }
                if (token == "Brans")
                {
                    profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                    branch = profileContent;
                    continue;
                }
            }
            readUserProfile.close();
            if (!isTeacher)
            {
                cout << "Soru ekleme yetkiniz yok!" << endl;
                continue;
            }

            cout << "Parola: ";
            cin >> password;

            string checkPassword = passwordCheck(passwordByDataBase, password);
            if (checkPassword == "failed")
            {
                cout << "Hatali parola girislerinden dolayi oturum acma islemi iptal edilmistir! Lutfen tekrar deneyiniz." << endl;
                continue;
            }

            cout << "\nAsagida bransiniza uygun dersler listelenmistir." << endl;

            string branchesWay = way + "/allLessons/branches/" + branch + "Lessons.txt";
            ifstream readBranchFile;
            int counter = 0;
            fileContent = "";
            readBranchFile.open(branchesWay, ios_base::out);
            while (getline(readBranchFile, fileContent))
            {
                counter++;
                cout << counter << ".) " << fileContent << endl;
            }
            readBranchFile.close();

            int courseNumber = 0;
            while (true)
            {
                cout << "Sorusunu eklemek istediginiz dersin numarasini giriniz: ";
                cin >> courseNumber;
                if (courseNumber > counter || courseNumber <= 0)
                {
                    cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
                    continue;
                }

                int meter = 0;
                fileContent = "";
                readBranchFile.open(branchesWay, ios_base::out);
                while (getline(readBranchFile, fileContent))
                {
                    meter++;
                    if (meter == courseNumber)
                    {
                        course = fileContent;
                        break;
                    }
                }
                readBranchFile.close();
                break;
            }
            cout << "course: " << course << endl;
            questionAppend(course);
        }
        else if (selectedAction == 2)
        {
            while (true)
            {
                int userPreference = 0;
                cout << "\n1.) Kayit Ol\n2.) Giris Yap\n3.) Ana Menu\nTercihiniz: ";
                cin >> userPreference;

                if (userPreference == 1)
                {
                    string authority = "";
                    cout << "Yapabileceginiz unvan tercihleri: ogretmen/teacher/t veya ogrenci/student/s" << endl;
                    while (true)
                    {
                        cout << "Unvaniniz: ";
                        cin >> authority;
                        authority = trim(authority);
                        authority = toLowercase(authority);

                        if (authority == "ogretmen" || authority == "teacher" || authority == "t")
                        {
                            int passwordForTeacherByUser = 0;
                            Teachers userTeacher;
                            cout << "Ogretmenlik unvanini secebilmeniz icin sistemin size verdigi sifreyi girmeniz gerekmektedir.\nSifre: ";
                            cin >> passwordForTeacherByUser;

                            string result = passwordCheck(userTeacher.getPasswordForTeacher(), passwordForTeacherByUser);
                            if (result == "failed")
                            {
                                cout << "Cok fazla hatali giris yaptiniz! Guncel sifreye sahip oldugunuzdan emin olunuz." << endl;
                                break;
                            }
                            cout << "Sifre dogru, ogretmenlik unvaninda kullanici olusturabilirsiniz." << endl;
                            userTeacher.title = "Ogretmen";
                            bool isRegistered = userTeacher.signUp();
                            if (isRegistered)
                                break;
                            cout << "Kullanici olusturma asamasinda bir hata olustu! Lutfen tekrar deneyiniz." << endl;
                        }
                        else if (authority == "ogrenci" || authority == "student" || authority == "s")
                        {
                            Students userStudent;
                            userStudent.title = "Ogrenci";
                            bool isRegistered = userStudent.signUp();
                            if (isRegistered)
                                break;
                            cout << "Kullanici olusturma asamasinda bir hata olustu! Lutfen tekrar deneyiniz." << endl;
                        }
                        else
                            cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
                    }
                    break;
                }
                else if (userPreference == 2)
                {
                    bool isLoggedIn = Users().signIn();
                    if (!isLoggedIn)
                        break;
                    if (isStarted)
                    {
                        string way = get_current_dir();
                        string userProfileWay = way + "/users/" + globalUsername + "/profile.txt";

                        string profileContent = "";
                        string delimiter = ": ";
                        bool isStudent = false;
                        ifstream readUserProfile;
                        readUserProfile.open(userProfileWay, ios_base::out);
                        while (getline(readUserProfile, profileContent))
                        {
                            string token = profileContent.substr(0, profileContent.find(delimiter));
                            if (token == "Unvan")
                            {
                                profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                                if (profileContent == "Ogrenci")
                                    isStudent = true;
                                continue;
                            }
                        }
                        readUserProfile.close();
                        if (!isStudent)
                        {
                            cout << "Sinavi sadece 'Ogrenci' unvanina sahip kullanicilar olabilir!" << endl;
                            break;
                        }

                        printf("\nAsagida dersleriniz listelenmistir,\n");
                        profileContent = "";
                        int counter = 0;
                        readUserProfile.open(userProfileWay, ios_base::out);
                        while (getline(readUserProfile, profileContent))
                        {
                            string token = profileContent.substr(0, profileContent.find(delimiter));
                            if (token == "Dersleri")
                            {
                                profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                                delimiter = ", ";
                                size_t pos = 0;
                                token = "";
                                while ((pos = profileContent.find(delimiter)) != string::npos)
                                {
                                    counter++;
                                    token = profileContent.substr(0, pos);
                                    cout << counter << ".) " << token << endl;
                                    profileContent.erase(0, pos + delimiter.length());
                                }
                                break;
                            }
                        }
                        readUserProfile.close();

                        int courseNumber = 0;
                        string course = "";
                        while (true)
                        {
                            cout << "Sinav olmak istediginiz dersin numarasini giriniz: ";
                            cin >> courseNumber;
                            if (courseNumber > counter || courseNumber <= 0)
                            {
                                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
                                continue;
                            }

                            int meter = 0;
                            profileContent = "";
                            delimiter = ": ";
                            readUserProfile.open(userProfileWay, ios_base::out);
                            while (getline(readUserProfile, profileContent))
                            {
                                string token = profileContent.substr(0, profileContent.find(delimiter));
                                if (token == "Dersleri")
                                {
                                    profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                                    delimiter = ", ";
                                    int pos = 0;
                                    token = "";
                                    while ((pos = profileContent.find(delimiter)) != string::npos)
                                    {
                                        meter++;
                                        token = profileContent.substr(0, pos);
                                        if (meter == courseNumber)
                                        {
                                            course = token;
                                            break;
                                        }
                                        profileContent.erase(0, pos + delimiter.length());
                                    }
                                    break;
                                }
                            }
                            readUserProfile.close();
                            break;
                        }
                        quizStart(course);
                        break;
                    }
                }
                else if (userPreference == 3)
                    break;
                else
                    cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
            }
        }
        else if (selectedAction == 3)
        {
            cout << string(50, '*') << " Uygulama Sonlandirildi " << string(50, '*') << endl;
            break;
        }
        else
            printf("Yanlis deger girdiniz. Lutfen tekrar deneyiniz.\n");
    }
}

int main()
{
    cout << string(50, '*') << " Sinav Uygulamasi " << string(50, '*') << endl;
    createMenu();
    return 0;
}