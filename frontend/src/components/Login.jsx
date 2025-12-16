import { useState } from 'react'
import { authAPI } from '../services/api'
import './Login.css'

const Login = ({ onLogin }) => {
  const [isRegister, setIsRegister] = useState(false)
  const [username, setUsername] = useState('')
  const [password, setPassword] = useState('')
  const [error, setError] = useState('')
  const [loading, setLoading] = useState(false)

  const handleSubmit = async (e) => {
    e.preventDefault()
    setError('')
    setLoading(true)

    try {
      if (isRegister) {
        await authAPI.register(username, password)
        const result = await authAPI.login(username, password)
        if (result.token) {
          onLogin(result)
        }
      } else {
        const result = await authAPI.login(username, password)
        if (result.token) {
          onLogin(result)
        } else {
          setError('Неверные учетные данные')
        }
      }
    } catch (err) {
      setError(err.response?.data?.error || 'Произошла ошибка')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="login-container">
      <div className="login-card">
        <h1>Менеджер задач</h1>
        <h2>{isRegister ? 'Регистрация' : 'Вход'}</h2>
        
        <form onSubmit={handleSubmit}>
          <div className="form-group">
            <label htmlFor="username">Имя пользователя</label>
            <input
              type="text"
              id="username"
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              required
              minLength={3}
              placeholder="Введите имя пользователя"
            />
          </div>
          
          <div className="form-group">
            <label htmlFor="password">Пароль</label>
            <input
              type="password"
              id="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              required
              minLength={3}
              placeholder="Введите пароль"
            />
          </div>
          
          {error && <div className="error-message">{error}</div>}
          
          <button type="submit" disabled={loading} className="submit-button">
            {loading ? 'Загрузка...' : (isRegister ? 'Зарегистрироваться' : 'Войти')}
          </button>
        </form>
        
        <p className="toggle-mode">
          {isRegister ? (
            <>Уже есть аккаунт? <button onClick={() => setIsRegister(false)}>Войти</button></>
          ) : (
            <>Нет аккаунта? <button onClick={() => setIsRegister(true)}>Зарегистрироваться</button></>
          )}
        </p>
      </div>
    </div>
  )
}

export default Login

