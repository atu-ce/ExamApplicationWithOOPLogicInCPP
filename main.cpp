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
string globalUsername;

void registrationFailed()
{
    isStarted = false;
}

void userCreated()
{
    isStarted = true;
}

string trim(const string &str,
            const string &whitespace = " \t")
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

bool IsPathExist(const string &s)
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
            if (counter == 3)
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
    int score;
    int totalTrueAnswer, totalFalseAnswer;
    vector<Questions> questions;
    int questionIndex;

public:
    Quiz(vector<Questions> comQuestions)
    {
        questions = comQuestions;
        score = 0;
        questionIndex = 0;
        totalTrueAnswer = 0;
        totalFalseAnswer = 0;
    }

    // Bu fonksiyonun amacı, o anki 'questionIndex'in değerine karşılık gelen  question objesini göndermek.
    Questions getQuestion()
    {
        return questions[questionIndex];
    }

    // Bu fonksiyon, gelen question objesini ekrana yazdırır.
    void disPlayQuestion()
    {
        Questions question = getQuestion();
        string answer;

        cout << "Soru " << (questionIndex + 1) << ": " << question.getQuestion() << "?\nCevap: ";
        getline(cin, answer);
        answer = toLowercase(answer);
        answer = trim(answer);
        guess(answer);
        loadQuestion();
    }

    // Bu fonksiyonun amacı, koşulsuz index numarasını arttırmak ve
    // 'checkAnswer'dan gelen değer 'True' ise score değiskenini arttırmak.
    void guess(string answer)
    {
        Questions question = getQuestion();

        if (question.checkAnswer(answer))
        {
            score++;
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
    void loadQuestion()
    {
        if (questions.size() == questionIndex)
        {
            createQuizResults();
            showScore();
        }
        else
        {
            disPlayProgress();
            disPlayQuestion();
        }
    }

    // Quizi bitirir ve kullanıcıya skorunu gösterir.
    void showScore()
    {
        cout << string(50, '*') << " Sonuclar " << string(50, '*') << endl;
        cout << "Sinav Bitti. Notunuz: " << score << endl;

        printf("\nDogru Cevaplariniz:\n");
        for (int i = 0; i < totalTrueAnswer; i++)
        {
            cout << trueAnswers[i] << endl;
        }

        printf("\nYanlis Cevaplariniz:\n");
        for (int j = 0; j < totalFalseAnswer; j++)
        {
            cout << falseAnswers[j] << endl;
        }

        cout << string(50, '*') << " Sinav Bitti " << string(50, '*') << endl;
    }

    // Kullanıcıya kaç sorudan kaçıncıda olduğunu gösterir.
    void disPlayProgress()
    {
        int totalQuestion = questions.size();
        int questionNumber = questionIndex + 1;
        cout << string(50, '*') << "Soru " << questionNumber << "/" << totalQuestion << string(50, '*') << endl;
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
    void createQuizResults()
    {
        string createdDate, createdTime, createdDateTime;

        time_t now = time(0);
        tm *ltm = localtime(&now);

        createdDate = to_string(ltm->tm_mday) + "_" + to_string(1 + ltm->tm_mon) + "_" + to_string(1900 + ltm->tm_year);
        createdTime = to_string(ltm->tm_hour) + "_" + to_string(ltm->tm_min) + "_" + to_string(ltm->tm_sec);
        createdDateTime = createdDate + "_" + createdTime;

        string way = get_current_dir() + "/users/" + globalUsername + "/" + createdDateTime + ".txt";

        ofstream dateTimeFile(way);
        dateTimeFile << "Dogru Cevaplariniz:" << endl;
        for (int i = 0; i < totalTrueAnswer; i++)
        {
            dateTimeFile << trueAnswers[i] << endl;
        }
        dateTimeFile << "\n";
        dateTimeFile << "Yanlis Cevaplariniz:" << endl;
        for (int j = 0; j < totalFalseAnswer; j++)
        {
            dateTimeFile << falseAnswers[j] << endl;
        }
        dateTimeFile.close();
    }
};

class People
{
protected:
    string name, surname, gender;
    int age;
};

class Users : public People
{
private:
    int passwordForTeacherByClass;

protected:
    int password, passwordAgain;
    string username, role;

public:
    string title;
    Users()
    {
        passwordForTeacherByClass = 112233;
    }

    int getPasswordForTeacher()
    {
        return passwordForTeacherByClass;
    }

    // Ana sınıfın signUp() fonksiyonudur kaydolma işlemleri türetilmiş sınıflarda yapılacaktır.
    virtual bool signUp()
    {
        string way = get_current_dir();
        string usersWay = way + "/users";

        bool isUsersFolder = IsPathExist(usersWay);

        if (!isUsersFolder)
        {
            int status = mkdir(usersWay.c_str(), 0777);
        }

        return false;
    }

    bool signIn()
    {
        string localUsername;
        int localPassword;

        cout << "Kullanici adi: ";
        cin >> localUsername;
        localUsername = trim(localUsername);
        localUsername = toLowercase(localUsername);

        string way = get_current_dir();
        string loginInquiryWay = way + "/loginInquiry.txt";

        bool isloginInquiryFile = IsPathExist(loginInquiryWay);

        if (!isloginInquiryFile)
        {
            ofstream loginInquiryFile(loginInquiryWay);
            loginInquiryFile.close();
        }

        string fileContent;
        ifstream readLoginInquiryFile(loginInquiryWay);

        while (getline(readLoginInquiryFile, fileContent))
        {
            if (localUsername == fileContent)
            {
                cout << "Parola: ";
                cin >> localPassword;

                string way = get_current_dir();
                string userProfileWay = way + "/users/" + localUsername + "/profile.txt";

                string profileContent;
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
    int currentClass, totalExams, finishedExams;
    friend string isPassed(Lessons lessons);

    Lessons(string comCourseName, int comCurrentClass, int comTotalExams, int comFinishedExams)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = comTotalExams;
        finishedExams = comFinishedExams;
    }
    Lessons(string comCourseName, int comCurrentClass, int comTotalExams)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = comTotalExams;
        finishedExams = 0;
    }
    Lessons(string comCourseName, int comCurrentClass)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        totalExams = 2;
        finishedExams = 0;
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
    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve bilgiler profile.txt dosyasına kaydedilir.
    bool signUp()
    {
        cout << "Sistemimiz asagida listelenmis bolumleri kapsamaktadir," << endl;
        cout << "1.) Bilgisayar Muhendisligi\n2.) Makine Muhendisligi" << endl;
        int branch;
        while (true)
        {
            cout << "Lutfen bolumunuzu seciniz: ";
            cin >> branch;
            if (branch == 1)
            {
                role = "BM";
                break;
            }
            else if (branch == 2)
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

        bool isUsersFolder = IsPathExist(usersWay);

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
                userProfile << "Isim: " << name << endl;
                userProfile << "Soyisim: " << surname << endl;
                userProfile << "Cinsiyet: " << gender << endl;
                userProfile << "Yas: " << age << endl;
                userProfile << "Unvaniniz: " << role << endl;
                userProfile << "Kullanici adi: " << username << endl;
                userProfile << "Parola: " << password << endl;
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

class Teachers : public Users
{
public:
    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve bilgiler profile.txt dosyasına kaydedilir.
    virtual bool signUp()
    {
        cout << "Sistemimiz asagida listelenmis branslari kapsamaktadir," << endl;
        cout << "1.) Matematik\n2.) Fizik\n3.) Bilgisayar Bolumu Ogretmeni\n4.) Makine Bolumu Ogretmeni" << endl;
        int branch;
        while (true)
        {
            cout << "Lutfen bransinizi seciniz: ";
            cin >> branch;
            if (branch == 1)
            {
                role = "Matematik";
                break;
            }
            else if (branch == 2)
            {
                role = "Fizik";
                break;
            }
            else if (branch == 3)
            {
                role = "Bilgisayar Bolumu";
                break;
            }
            else if (branch == 4)
            {
                role = "Makine Bolumu";
                break;
            }
            else
                cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
        }

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

        bool isUsersFolder = IsPathExist(usersWay);

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
                userProfile << "Isim: " << name << endl;
                userProfile << "Soyisim: " << surname << endl;
                userProfile << "Cinsiyet: " << gender << endl;
                userProfile << "Yas: " << age << endl;
                userProfile << "Unvan: " << role << endl;
                userProfile << "Brans: " << role << endl;
                userProfile << "Kullanici adi: " << username << endl;
                userProfile << "Parola: " << password << endl;
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
vector<string> findLessons(int whichGrader, string role)
{
    int counter = 0;
    vector<string> lessons;

    string way = get_current_dir() + "/allLessons/";
    string departmentLessonsWay = way + role + "Lessons.txt";

    string fileContent;
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

vector<Lessons> createLessonObjects(int whichGrader, string role)
{
    vector<string> lessons = findLessons(whichGrader, role);
    int lessonsSize = lessons.size();
    vector<Lessons> lessonObjects;
    for (string lesson : lessons)
    {
        Lessons lessonObject = Lessons(lesson, whichGrader);
        lessonObjects.push_back(lessonObject);
    }
    return lessonObjects;
}

// 1'e basıldığında bu fonksiyon çalışır. Yeni soru ekler.
void questionAppend()
{
    string question, answer;

    cout << "Soru: ";
    getline(cin, question);
    question = trim(question);
    question = toLowercase(question);

    cout << "Cevap: ";
    getline(cin, answer);
    answer = trim(answer);
    answer = toLowercase(answer);

    string way = get_current_dir();
    string questionsAndAnswersWay = way + "/questionsAndAnswers.txt";

    ofstream questionsAndAnswersFile;
    questionsAndAnswersFile.open(questionsAndAnswersWay, ios_base::app);
    questionsAndAnswersFile << question << ": " << answer << endl;
    questionsAndAnswersFile.close();
}

// 2'ye basıldığında bu fonksiyon çalışır. Sınavı başlatır.
void quizStart()
{
    vector<Questions> questions;
    string way = get_current_dir();
    string questionsAndAnswersWay = way + "/questionsAndAnswers.txt";

    string fileContent;
    string delimiter = ": ";
    ifstream readQuestionsAndAnswersFile(questionsAndAnswersWay);

    while (getline(readQuestionsAndAnswersFile, fileContent))
    {
        string token = fileContent.substr(0, fileContent.find(delimiter));
        fileContent.erase(0, fileContent.find(delimiter) + delimiter.length());
        questions.push_back(Questions(token, fileContent));
    }

    readQuestionsAndAnswersFile.close();

    Quiz quiz(questions);
    quiz.loadQuestion();
}

// Menünün oluşturulduğu bölüm
void createMenu()
{
    int selectedAction;
    while (true)
    {
        cout << "\n1.) Soru Ekle\n2.) Sinava Basla\n3.) Cikis\nTercihiniz: ";
        cin >> selectedAction;

        if (selectedAction == 1)
        {
            string access;
            int password;
            // Soru eklemeyi sadece öğretmenlerin yapabilmesi için gerekli kod dizini.
            cout << "Kullanici adi: ";
            cin >> access;
            access = trim(access);
            access = toLowercase(access);

            cout << "Parola: ";
            cin >> password;

            // ERİŞİM YETKİSİ KONTROLÜ YAPILACAK!
            if (access == "admin")
            {
                questionAppend();
            }
            else
                cout << ">>> Soru ekleme yetkiniz yok! <<<" << endl;
        }
        else if (selectedAction == 2)
        {
            while (true)
            {
                int userPreference;
                cout << "\n1.) Kayit Ol\n2.) Giris Yap\n3.) Ana Menu\nTercihiniz: ";
                cin >> userPreference;

                if (userPreference == 1)
                {
                    string authority;
                    cout << "Yapabileceginiz unvan tercihleri: ogretmen/teacher/t veya ogrenci/student/s" << endl;
                    while (true)
                    {
                        cout << "Unvaniniz: ";
                        cin >> authority;
                        authority = trim(authority);
                        authority = toLowercase(authority);

                        if (authority == "ogretmen" || authority == "teacher" || authority == "t")
                        {
                            int passwordForTeacherByUser;
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
                            userTeacher.title = "ogretmen";
                            bool isRegistered = userTeacher.signUp();
                            if (isRegistered)
                                break;
                            cout << "Kullanici olusturma asamasinda bir hata olustu! Lutfen tekrar deneyiniz." << endl;
                        }
                        else if (authority == "ogrenci" || authority == "student" || authority == "s")
                        {
                            Students userStudent;
                            userStudent.title = "ogrenci";
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
                    if (isLoggedIn)
                        break;
                }
                else if (userPreference == 3)
                    break;
                else
                    cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
            }
            if (isStarted)
            {
                quizStart();
                break;
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
    // vector<string> bmLessons=findLessons(4,"BM");
    // vector<string> mmLessons=findLessons(3,"MM");
    // for (int i = 0; i < bmLessons.size(); i++)
    // {
    //     cout<<bmLessons[i]<<endl;
    // }
    // printf("\n");
    // for (int j = 0; j < mmLessons.size(); j++)
    // {
    //     cout<<mmLessons[j]<<endl;
    // }
    cout << string(50, '*') << " Sinav Uygulamasi " << string(50, '*') << endl;
    createMenu();
    return 0;
}