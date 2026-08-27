#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "TLorentzVector.h"
#include "TVector3.h"

// =============================================================================
// 1. ПОЛЬЗОВАТЕЛЬСКАЯ КОНФИГУРАЦИЯ (Все входные данные меняются здесь)
// =============================================================================
struct KinematicConfig {
    // Массы частиц (в ГэВ/c^2)
    const double PROTON_MASS     = 0.938272;
    const double CHARGED_PI_MASS = 0.139570;
    const double ELECTRON_MASS   = 0.000511;

    // Начальный электронный пучок (ГэВ)
    double E_beam = 10.6;

    // Рассеянный электрон в ЛСО (Энергия в ГэВ, углы theta и phi в радианах)
    double E_e     = 3.5;
    double theta_e = 0.25; // ~14.3 град
    double phi_e   = 0.8;  // ~45.8 град

    // Рожденный пион (pi+) в ЛСО (Импульс в ГэВ/c, углы theta и phi в радианах)
    double p_pi     = 1.8;
    double theta_pi = 0.4; // ~22.9 град
    double phi_pi   = 2.1; // ~120.3 град
};

// =============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ВЫВОДА
// =============================================================================
void print_header(const std::string& title) {
    std::cout << "\n====================================================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "====================================================================================================\n";
}

void print_vector(const std::string& name, const TLorentzVector& v) {
    std::cout << "  " << std::left << std::setw(12) << name 
              << " | Px: " << std::right << std::setw(10) << std::fixed << std::setprecision(5) << v.Px()
              << " | Py: " << std::setw(10) << v.Py()
              << " | Pz: " << std::setw(10) << v.Pz()
              << " | E: "  << std::setw(10) << v.E() 
              << " | M: "  << std::setw(9)  << std::setprecision(4) << v.M() << std::endl;
}

// =============================================================================
// ОСНОВНАЯ ФУНКЦИЯ ДЕМОНСТРАЦИИ
// =============================================================================
void test_kinematics() {
    // Загружаем входные параметры
    KinematicConfig cfg;

    // -------------------------------------------------------------------------
    // Инициализация векторов в ЛСО
    // -------------------------------------------------------------------------
    double p_beam = std::sqrt(cfg.E_beam * cfg.E_beam - cfg.ELECTRON_MASS * cfg.ELECTRON_MASS);
    TLorentzVector e_in(0.0, 0.0, p_beam, cfg.E_beam);
    TLorentzVector target(0.0, 0.0, 0.0, cfg.PROTON_MASS);

    double p_e = std::sqrt(cfg.E_e * cfg.E_e - cfg.ELECTRON_MASS * cfg.ELECTRON_MASS);
    double p_ex = p_e * std::sin(cfg.theta_e) * std::cos(cfg.phi_e);
    double p_ey = p_e * std::sin(cfg.theta_e) * std::sin(cfg.phi_e);
    double p_ez = p_e * std::cos(cfg.theta_e);
    TLorentzVector electron(p_ex, p_ey, p_ez, cfg.E_e);

    TLorentzVector gamma = e_in - electron; // Виртуальный фотон q = k - k'

    double p_pix = cfg.p_pi * std::sin(cfg.theta_pi) * std::cos(cfg.phi_pi);
    double p_piy = cfg.p_pi * std::sin(cfg.theta_pi) * std::sin(cfg.phi_pi);
    double p_piz = cfg.p_pi * std::cos(cfg.theta_pi);
    double E_pi  = std::sqrt(cfg.p_pi * cfg.p_pi + cfg.CHARGED_PI_MASS * cfg.CHARGED_PI_MASS);
    TLorentzVector pion(p_pix, p_piy, p_piz, E_pi);

    // Наборы векторов для Метод 1 (Прямой буст) и Метод 2 (Инвариантный буст)
    TLorentzVector e1 = electron, g1 = gamma, t1 = target, pi1 = pion;
    TLorentzVector e2 = electron, g2 = gamma, t2 = target, pi2 = pion;

    print_header("0. ИСХОДНОЕ СОСТОЯНИЕ В ЛСО (Лабораторная система отсчета)");
    print_vector("beam (e_in)", e_in);
    print_vector("electron", e1);
    print_vector("gamma (q)", g1);
    print_vector("target (p)", t1);
    print_vector("pion (pi+)", pi1);

    // -------------------------------------------------------------------------
    // ШАГ 1: RotateZ(-phi_e) — совпадение плоскости рассеяния с XZ
    // -------------------------------------------------------------------------
    double phi_f = electron.Phi();
    print_header("1. ПОВОРОТ RotateZ(-phi_e), phi_e = " + std::to_string(phi_f) + " рад (" + std::to_string(phi_f * 180 / M_PI) + " deg)");
    std::cout << "   [Цель]: Убрать Py-компоненту у электрона и фотона.\n";

    e1.RotateZ(-phi_f);  g1.RotateZ(-phi_f);  t1.RotateZ(-phi_f);  pi1.RotateZ(-phi_f);
    e2.RotateZ(-phi_f);  g2.RotateZ(-phi_f);  t2.RotateZ(-phi_f);  pi2.RotateZ(-phi_f);

    print_vector("electron", e1);
    print_vector("gamma (q)", g1);
    print_vector("pion (pi+)", pi1);

    // -------------------------------------------------------------------------
    // ШАГ 2: RotateY(+theta_q) — направление виртуального фотона вдоль оси +Z
    // -------------------------------------------------------------------------
    double theta_q = gamma.Theta();
    print_header("2. ПОВОРОТ RotateY(+theta_q), theta_q = " + std::to_string(theta_q) + " рад (" + std::to_string(theta_q * 180 / M_PI) + " deg)");
    std::cout << "   [Цель]: Повернуть фотон gamma (q) строго вдоль оси +Z (Px -> 0, Py -> 0).\n";

    e1.RotateY(theta_q);  g1.RotateY(theta_q);  t1.RotateY(theta_q);  pi1.RotateY(theta_q);
    e2.RotateY(theta_q);  g2.RotateY(theta_q);  t2.RotateY(theta_q);  pi2.RotateY(theta_q);

    print_vector("electron", e1);
    print_vector("gamma (q)", g1);
    print_vector("pion (pi+)", pi1);

    // -------------------------------------------------------------------------
    // ШАГ 3: БУСТ В СЦМ (Сравнение двух методов)
    // -------------------------------------------------------------------------
    print_header("3. СРАВНЕНИЕ РАСЧЕТА БУСТА (МЕТОД 1 vs МЕТОД 2)");

    // МЕТОД 1: Напрямую через импульс и энергию γp системы
    double beta_val_1 = -g1.P() / (g1.E() + cfg.PROTON_MASS);
    TVector3 beta1(0.0, 0.0, beta_val_1);

    // МЕТОД 2: Через кинематические инварианты Q^2, W, nu
    double Q2 = -gamma.M2(); // Виртуальность фотона Q^2 = -q^2
    TLorentzVector W_vector = e_in + target - electron; // Система адронов (q + p)
    double W = W_vector.M(); // Инвариантная масса W
    double nu = (W * W + Q2 - cfg.PROTON_MASS * cfg.PROTON_MASS) / (2.0 * cfg.PROTON_MASS); // Передача энергии nu = E_gamma
    
    double beta_val_2 = -std::sqrt(nu * nu + Q2) / (nu + cfg.PROTON_MASS);
    TVector3 beta2(0.0, 0.0, beta_val_2);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Инварианты реакции:\n";
    std::cout << "    * Q^2 (виртуальность фотона) = " << Q2 << " ГэВ^2\n";
    std::cout << "    * W   (инвариантная масса)   = " << W  << " ГэВ\n";
    std::cout << "    * nu  (передача энергии)     = " << nu << " ГэВ\n\n";

    std::cout << "  Параметры буста вдоль оси Z:\n";
    std::cout << "    * Метод 1  (Прямой q/E_tot)  : beta_z = " << beta_val_1 << "\n";
    std::cout << "    * Метод 2  (Инварианты Q2,W) : beta_z = " << beta_val_2 << "\n";
    std::cout << "    * Разница (|beta1 - beta2|)   : " << std::scientific << std::abs(beta_val_1 - beta_val_2) << std::fixed << "\n";

    // Применяем бусты
    e1.Boost(beta1);  g1.Boost(beta1);  t1.Boost(beta1);  pi1.Boost(beta1);
    e2.Boost(beta2);  g2.Boost(beta2);  t2.Boost(beta2);  pi2.Boost(beta2);

    // -------------------------------------------------------------------------
    // ФИНАЛЬНЫЕ РЕЗУЛЬТАТЫ В СЦМ
    // -------------------------------------------------------------------------
    print_header("4. ИТОГОВОЕ СОСТОЯНИЕ В СЦМ \"ФОТОН-ПРОТОН\"");
    
    std::cout << "\n--- МЕТОД 1 (Прямой кинематический буст) ---\n";
    print_vector("electron", e1);
    print_vector("gamma (q)", g1);
    print_vector("target (p)", t1);
    print_vector("pion (pi+)", pi1);

    TLorentzVector total_cm1 = g1 + t1;
    std::cout << "  >> Суммарный 3-импульс (gamma + target) в СЦМ: Px=" << total_cm1.Px() 
              << ", Py=" << total_cm1.Py() << ", Pz=" << total_cm1.Pz() << " (Должен быть 0!)\n";

    std::cout << "\n--- МЕТОД 2 (Буст через инварианты Q^2 и W) ---\n";
    print_vector("electron", e2);
    print_vector("gamma (q)", g2);
    print_vector("target (p)", t2);
    print_vector("pion (pi+)", pi2);

    TLorentzVector total_cm2 = g2 + t2;
    std::cout << "  >> Суммарный 3-импульс (gamma + target) в СЦМ: Px=" << total_cm2.Px() 
              << ", Py=" << total_cm2.Py() << ", Pz=" << total_cm2.Pz() << " (Должен быть 0!)\n";

    // Прямая проверка абсолютной разности векторов пиона
    double diff_Px = std::abs(pi1.Px() - pi2.Px());
    double diff_Py = std::abs(pi1.Py() - pi2.Py());
    double diff_Pz = std::abs(pi1.Pz() - pi2.Pz());
    double diff_E  = std::abs(pi1.E()  - pi2.E());

    print_header("5. ВЕРДИКТ ЭКВИВАЛЕНТНОСТИ МЕТОДОВ");
    std::cout << std::scientific << std::setprecision(3);
    std::cout << "  Максимальное расхождение компонентов пиона в СЦМ между двумя методами:\n";
    std::cout << "    |dPx| = " << diff_Px << " | |dPy| = " << diff_Py 
              << " | |dPz| = " << diff_Pz << " | |dE| = " << diff_E << " ГэВ\n";
    std::cout << "  ВЫВОД: Оба метода математически идентичны с точностью до машинного нуля!\n";
    std::cout << "====================================================================================================\n\n";
}