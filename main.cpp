// Uygulamanın çalışması için gerekli kütüphaneler.
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include <algorithm>

// İşletim sistemine göre uygun kütüphaneyi include (dahil) edecek kod dizisi.
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace std;

// Global olarak tanımlanan değişkenler.
bool isStarted = false;
string globalUsername = "";

// Uygulamaya giriş başarısız olduğunda bu fonksiyon çağrılacak, 'isStarted' değerini 'false' yapan fonksiyon.
void registrationFailed()
{
    isStarted = false;
}

// Uygulamaya giriş başarılı olduğunda bu fonksiyon çağrılacak, 'isStarted' değerini 'true' yapan fonksiyon.
void userCreated()
{
    isStarted = true;
}

// Sağda ve soldaki boşlukları kırpan fonksiyon.
string trim(const string &str, const string &whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

// Parametre olarak verilen string tipindeki değişkenin içeriğindeki karakterleri küçük harfe dönüştüren fonksiyon.
string toLowercase(string data)
{
    for_each(data.begin(), data.end(), [](char &c)
             { c = ::tolower(c); });
    return data;
}

// Bulunduğu dizinin adres yolunu döndüren fonksiyon.
string get_current_dir()
{
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

// Parametre olarak verilen string tipindeki adres yolunda, belirtilen klasörün yada dosyanın olup olmadığını kontrol eder.
bool isPathExist(const string &s)
{
    struct stat buffer;
    return (stat(s.c_str(), &buffer) == 0);
}

// Parametre olarak verilen iki password bilgisi aynı olup olmadığını kontrol eden fonksiyon.
string passwordCheck(int firstPassword, int secondPassword)
{
    int counter = 0;
    if (firstPassword != secondPassword)
    {
        while (true)
        {
            if (counter == 2)
            {
                cout << "\n3 defa yanlis parola girdiniz!" << endl;
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
    // Yapıcı (Constructor) fonksiyon.
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

    // Private erişim iznine sahip 'answer' değişkenine dışarıdan erişimi sağlayan fonksiyon.
    string getAnswer()
    {
        return answer;
    }

    // Protected erişim iznine sahip 'text' değişkenine dışarıdan erişimi sağlayan fonksiyon.
    string getQuestion()
    {
        return text;
    }
};

class Lessons
{
protected:
    string letterGrade;
    int note;

public:
    string courseName;
    int currentClass;

    // ** Çok biçimlilik (Polymorphism) yapısı kullanıldı. **
    // Constructor (Yapıcı) fonksiyon 1.
    Lessons(string comCourseName)
    {
        courseName = comCourseName;
        currentClass = 0;
        letterGrade = "";
        note = 0;
    }

    // Constructor (Yapıcı) fonksiyon 2.
    Lessons(string comCourseName, int comCurrentClass)
    {
        courseName = comCourseName;
        currentClass = comCurrentClass;
        letterGrade = "";
        note = 0;
    }

    // Sınav notunuza göre harf notunu hesaplayan fonksiyon.
    bool toLetterGrade()
    {
        if (note >= 90)
            letterGrade = "AA";
        else if (note >= 85)
            letterGrade = "BA";
        else if (note >= 80)
            letterGrade = "BB";
        else if (note >= 75)
            letterGrade = "CB";
        else if (note >= 65)
            letterGrade = "CC";
        else if (note >= 58)
            letterGrade = "DC";
        else if (note >= 50)
            letterGrade = "DD";
        else if (note >= 40)
            letterGrade = "FD";
        else if (note < 40)
            letterGrade = "FF";
        else
            return false;
        return true;
    }

    // Protected erişim iznine sahip 'note' değişkenine dışarıdan atama yapılmasını sağlayan fonksiyon.
    void setNote(int comNote) { note = comNote; }

    // Protected erişim iznine sahip 'letterGrade' değişkenine dışarıdan erişimi sağlayan fonksiyon.
    string getLetterGrade()
    {
        return letterGrade;
    }

    // Friend yapısı kullanıldı.
    friend string isPassed(Lessons lesson);
};

// Gerekli prototipler (bildirimler) yapıldı.
vector<Lessons> createLessonObjects(int whichGrader, string role);
string isPassed(Lessons lesson);

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
    // Constructor (Yapıcı) Fonksiyon.
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

    // Sınav bittikten sonra kullanıcıya hangi soruya doğru, hangisine yanlış cevap verdiğini göstermeliyiz;
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

    // Sınav bitiminde bu fonksiyon çalışır, giriş yapan kullanıcının klasörüne o anki 'tarih-saat.txt' dosyası oluşturulur ve notu, doğru-yanlış cevap tablosu dosyaya eklenir.
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
            int status = mkdir(way.c_str(), 0777);

        for (int i = 1; i <= 2; i++)
        {
            string resultsWay = "";
            bool condition = false, isRealResult = false;
            if (i == 1)
            {
                resultsWay = way + "/realResult.txt";

                bool isRealResultFile = isPathExist(resultsWay);

                if (!isRealResultFile)
                {
                    condition = true;
                    isRealResult = true;
                }
            }
            else
            {
                resultsWay = way + "/" + createdDateTime + ".txt";
                condition = true;
            }
            if (condition)
            {
                ofstream resultsFile(resultsWay);

                resultsFile << "\n"
                            << string(40, '*') << " Sinav Sonucu " << string(46, '*') << endl;

                if (isRealResult)
                    resultsFile << course << " dersinden aldiginiz orjinal notunuzdur." << endl;
                else
                    resultsFile << course << " dersinden aldiginiz deneme notunuzdur. Bu notun bir hukmu yoktur!" << endl;

                Lessons lesson = Lessons(course);
                lesson.setNote(score);
                bool isAssign = lesson.toLetterGrade();

                resultsFile << "Notunuz: " << score << endl;
                resultsFile << "Harf Notunuz: " << lesson.getLetterGrade() << endl;
                resultsFile << "\nDogru Cevaplariniz:" << endl;
                for (int i = 0; i < totalTrueAnswer; i++)
                {
                    resultsFile << " > " << trueAnswers[i] << endl;
                }
                if (totalTrueAnswer == 0)
                    resultsFile << " > Dogru cevabiniz yok." << endl;
                resultsFile << "\n";
                resultsFile << "Yanlis Cevaplariniz:" << endl;
                for (int j = 0; j < totalFalseAnswer; j++)
                {
                    resultsFile << " > " << falseAnswers[j] << endl;
                }
                if (totalFalseAnswer == 0)
                    resultsFile << " > Yanlis cevabiniz yok." << endl;
                resultsFile << "\nSonuc: " << isPassed(lesson) << endl;
                resultsFile << "\n"
                            << string(100, '*') << endl;
                resultsFile.close();
            }
        }
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

// Scope resolution operator yapısı kullanıldı.
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

    // Constructor (Yapıcı) fonksiyon.
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

    // Virtual yapısı kullanıldı.
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

    // Kullanıcı adı ve parola istenir, değerler doğru ise uygulamaya girişi sağlanır.
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

class Students : public Users
{
protected:
    int whichGrader;
    int totalLessons;
    vector<Lessons> lessons;

public:
    Students();

    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve  bu bilgiler 'profile.txt' dosyasına kaydedilir.
    bool signUp()
    {
        cout << "\nSistemimiz asagida listelenmis bolumleri kapsamaktadir," << endl;
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

        cout << "\nSistemimiz sadece rakamlardan olusan parolaya izin vermektedir!" << endl;

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

        // try-catch yapısı kullanıldı.
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

// Scope resolution operator kullanılarak Students() yapıcı fonksiyonunun yapacağı işlemler belirtildi.
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
    // Constructor (Yapıcı) fonksiyon.
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

    // Private erişim iznine sahip 'passwordForTeacherByClass' değişkenine dışarıdan erişimi sağlayan fonksiyon.
    int getPasswordForTeacher()
    {
        return passwordForTeacherByClass;
    }

    // Kullanıcıdan gerekli bilgiler istenir, gelen değer ile kullanıcı oluşturulur ve bu bilgiler 'profile.txt' dosyasına kaydedilir.
    bool signUp()
    {
        cout << "\nSistemimiz asagida listelenmis branslari kapsamaktadir," << endl;
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

        cout << "\nSistemimiz sadece rakamlardan olusan parolaya izin vermektedir!" << endl;

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

// Harf notuna göre 'gecti', 'kosullu gecti' yada 'kaldi' sonuçlarından birini döndüren fonksiyon.
string isPassed(Lessons lesson)
{
    if (lesson.letterGrade == "FF")
        return "kaldi";
    else if (lesson.letterGrade == "DC" || lesson.letterGrade == "DD" || lesson.letterGrade == "FD")
        return "kosullu gecti";
    return "gecti";
}

// ** Çok biçimlilik (Polymorphism) yapısı kullanıldı. **
// Parametre olarak verilen sınıf ve branş bilgisine göre o sınıfın derslerini döndüren fonksiyon.
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

// Parametre olarak gönderilen dersin istenilen sınav sonucunu ekrana yazdıran fonksiyon.
bool showQuizResult(string course)
{
    string way = get_current_dir() + "/users/" + globalUsername + "/" + course;

    cout << "\n1.) Orjinal/Gercek Notu Goruntule\n2.) Tarih Ve Saate Gore Deneme Notunu Goruntule\n3.) Geri Git" << endl;

    int choice = 0;
    while (true)
    {
        cout << "Birini seciniz: ";
        cin >> choice;
        if (choice == 1)
        {
            string realResultWay = way + "/realResult.txt";
            bool isRealResultFile = isPathExist(realResultWay);

            if (isRealResultFile)
            {
                string fileContent = "";
                ifstream readRealResultFile(realResultWay);
                while (getline(readRealResultFile, fileContent))
                {
                    cout << fileContent << endl;
                }
                readRealResultFile.close();
                return true;
            }
            else
            {
                cout << "Sinav olmadiginiz icin sinav sonucunuz yok!" << endl;
                return false;
            }
        }
        else if (choice == 2)
        {
            string quizDate = "", quizTime = "";
            while (true)
            {
                cout << "\nGeri donmek icin 'geri' veya 'g' yaziniz\n"
                     << endl;
                cout << "Sinav oldugunuz tarihi sirasiyla Gun_Ay_Yil biciminde giriniz.\nTarih: ";
                cin >> quizDate;

                if (quizDate == "geri" || quizDate == "g")
                {
                    cout << "\n1.) Orjinal/Gercek Notu Goruntule\n2.) Tarih Ve Saate Gore Deneme Notunu Goruntule\n3.) Geri Git" << endl;
                    break;
                }

                cout << "Sinav oldugunuz saati sirasiyla Saat_Dakika_Saniye biciminde giriniz.\nSaat: ";
                cin >> quizTime;

                string quizDateTime = quizDate + "_" + quizTime;
                string dateTimeResultWay = way + "/" + quizDateTime + ".txt";
                bool isDateTimeFile = isPathExist(dateTimeResultWay);

                if (isDateTimeFile)
                {
                    string fileContent = "";
                    ifstream readDateTimeFile(dateTimeResultWay);
                    while (getline(readDateTimeFile, fileContent))
                    {
                        cout << fileContent << endl;
                    }
                    readDateTimeFile.close();
                    return true;
                }
                else
                {
                    cout << "Yanlis deger/degerler girdiniz! Lutfen tekrar deneyiniz." << endl;
                    continue;
                }
            }
        }
        else if (choice == 3)
            return false;
        else
            cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
    }
    return false;
}

// Uygulamanın menüsünü oluşturan fonksiyon.
void createMenu()
{
    while (true)
    {
        int selectedAction = 0;
        cout << "\n1.) Kayit Ol\n2.) Giris Yap\n3.) Cikis Yap\nTercihiniz: ";
        cin >> selectedAction;

        if (selectedAction == 1)
        {
            string authority = "";
            cout << "\nYapabileceginiz unvan tercihleri: ogretmen/teacher/t veya ogrenci/student/s" << endl;
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
                    cout << "\nOgretmenlik unvanini secebilmeniz icin sistemin size verdigi sifreyi girmeniz gerekmektedir.\nSifre: ";
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
        }
        else if (selectedAction == 2)
        {
            bool isLoggedIn = Users().signIn();
            if (!isLoggedIn)
                continue;
            if (isStarted)
            {
                string way = get_current_dir();
                string userProfileWay = way + "/users/" + globalUsername + "/profile.txt";

                string profileContent = "";
                string delimiter = ": ";
                bool isTeacher = false;
                ifstream readUserProfile;
                readUserProfile.open(userProfileWay, ios_base::out);
                while (getline(readUserProfile, profileContent))
                {
                    string token = profileContent.substr(0, profileContent.find(delimiter));
                    if (token == "Unvan")
                    {
                        profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                        if (profileContent == "Ogretmen")
                            isTeacher = true;
                        continue;
                    }
                }
                readUserProfile.close();
                int userPreference = 0;
                if (isTeacher)
                {
                    while (true)
                    {
                        cout << "\n1.) Soru Ekle\n2.) Ana Menü\n3.) Cikis Yap\nTercihiniz: ";
                        cin >> userPreference;

                        if (userPreference == 1)
                        {
                            string branch = "", course = "";
                            delimiter = ": ";
                            profileContent = "";
                            readUserProfile.open(userProfileWay, ios_base::out);
                            while (getline(readUserProfile, profileContent))
                            {
                                string token = profileContent.substr(0, profileContent.find(delimiter));
                                if (token == "Brans")
                                {
                                    profileContent.erase(0, profileContent.find(delimiter) + delimiter.length());
                                    branch = profileContent;
                                    break;
                                }
                            }
                            readUserProfile.close();

                            cout << "\nAsagida bransiniza uygun dersler listelenmistir." << endl;

                            string branchesWay = way + "/allLessons/branches/" + branch + "Lessons.txt";
                            ifstream readBranchFile;
                            int counter = 0;
                            string fileContent = "";
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
                            questionAppend(course);
                        }
                        else if (userPreference == 2 || userPreference == 3)
                            break;
                        else
                            cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
                    }
                    if (userPreference == 3)
                    {
                        cout << string(40, '*') << " Uygulama Sonlandirildi " << string(36, '*') << endl
                             << endl;
                        break;
                    }
                }
                else
                {
                    while (true)
                    {
                        cout << "\n1.) Sinav Ol\n2.) Sonucu Goruntule\n3.) Ana Menü\n4.) Cikis Yap\nTercihiniz: ";
                        cin >> userPreference;
                        string bug = "";
                        getline(cin, bug); // getline() fonksiyonunun bir bug'ından dolayı bu satır yazılmıştır.

                        if (userPreference == 1 || userPreference == 2)
                        {
                            printf("\nAsagida dersleriniz listelenmistir,\n");
                            profileContent = "";
                            int counter = 0;
                            delimiter = ": ";
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
                                if (userPreference == 1)
                                    cout << "Sinav olmak istediginiz dersin numarasini giriniz: ";
                                else
                                    cout << "Sonucunu goruntulemek istediginiz dersin numarasini giriniz: ";

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
                            if (userPreference == 1)
                                quizStart(course);
                            else
                            {
                                bool isShowQuizResult = showQuizResult(course);
                                if (!isShowQuizResult)
                                    continue;
                            }
                        }
                        else if (userPreference == 3 || userPreference == 4)
                            break;
                        else
                            cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
                    }
                    if (userPreference == 4)
                    {
                        cout << string(40, '*') << " Uygulama Sonlandirildi " << string(36, '*') << endl
                             << endl;
                        break;
                    }
                }
            }
        }
        else if (selectedAction == 3)
        {
            cout << string(40, '*') << " Uygulama Sonlandirildi " << string(36, '*') << endl
                 << endl;
            break;
        }
        else
            cout << "Yanlis deger girdiniz! Lutfen tekrar deneyiniz." << endl;
    }
}

int main()
{
    cout << string(40, '*') << " Sinav Uygulamasi " << string(42, '*') << endl;
    createMenu();
    return 0;
}